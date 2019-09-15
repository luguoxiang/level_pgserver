#include <cassert>
#include <limits>
#include <absl/strings/match.h>

#include "WorkThreadInfo.h"

#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "common/ParseTools.h"

#include "execution/ExecutionException.h"
#include "execution/BasePlan.h"

#include "planbuilder/BuildPlan.h"

#include "WorkloadResult.h"

WorkThreadInfo::WorkThreadInfo(int iIndex) :
		m_iIndex(iIndex), m_rewritter(m_result), m_protocol(iIndex) {
	m_bTerminate.store(false);
	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}

	m_handler['S'] = [this] (PgDataReader& receiver) {
		m_protocol.sendSync(m_iAcceptFd);
	};

	m_handler['Q'] =  [this] (PgDataReader& receiver) {
		handleQuery(receiver);
	};

	m_handler['P'] = [this] (PgDataReader& receiver) {
		handleParse(receiver);
	};
	m_handler['B'] = [this] (PgDataReader& receiver) {
		handleBind(receiver);
	};

	m_handler['D'] = [this] (PgDataReader& receiver) {
		m_protocol.readColumnDescribeInfo(receiver);
		m_protocol.sendColumnDescription(m_pPlan.get());
		m_protocol.flushSend(m_iAcceptFd);
	};

	m_handler['E'] = [this] (PgDataReader& receiver) {
		handleExecute();
	};
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
}

void WorkThreadInfo::cancel(bool planOnly) {
	LOG(INFO)<< "Cancel worker" << m_iIndex;
	m_bTerminate.store(true);

	if(!planOnly) {
		::shutdown(m_iAcceptFd, SHUT_RD);
	}
}

void WorkThreadInfo::parse(const std::string_view sql) {
	if (absl::StartsWithIgnoreCase(sql, "DEALLOCATE")) {
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "SET ")) {
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "BEGIN")) {
		LOG(WARNING)<< "Transaction is not supported";
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "COMMIT")) {
		LOG(WARNING) << "Transaction is not supported";
		m_result.m_pResult = nullptr;
	} else {
		++m_iSqlCount;
		parseSql(&m_result, sql);

		m_result.m_pResult = m_rewritter.rewrite(m_result.m_pResult);
		if (m_result.m_pResult == nullptr) {
			throw ParseException(&m_result);
		}

	}
}

void WorkThreadInfo::resolve() {
	auto pTree = m_result.m_pResult;
	if (pTree == nullptr) {
		m_pPlan.reset(new EmptyPlan());
	} else {
#ifndef NDEBUG
		printTree(pTree, 0);
#endif
		if (pTree->m_type != NodeType::PLAN) {
			PARSE_ERROR("WRONG NODE ", pTree->m_sExpr);
		}
		if (pTree->getOp() == Operation::WORKLOAD) {
			m_pPlan = ExecutionPlanPtr(new WorkloadResult());
		} else {
			m_pPlan = buildPlan(pTree);
		}
	}
}

void WorkThreadInfo::handleQuery(PgDataReader& receiver) {

	auto sql = m_protocol.readQueryInfo(receiver);

	parse(sql);

	resolve();

	m_protocol.sendColumnDescription(m_pPlan.get());

	m_protocol.flushSend(m_iAcceptFd);

	m_bParsed = false;
	m_bBinded = false;

	handleExecute();
}

void WorkThreadInfo::handleParse(PgDataReader& receiver) {
	auto [sql, iParamNum] = m_protocol.readParseInfo(receiver);

	parse (sql);

	size_t iActualParamNum = m_result.m_bindParamNodes.size();
	if (iParamNum != iActualParamNum) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iParamNum,
				", actual ", iActualParamNum);
	}
	m_bParsed = true;
	m_bBinded = false;
}

void WorkThreadInfo::handleBind(PgDataReader& receiver) {
	m_result.mark();

	size_t iActualNum = m_result.m_bindParamNodes.size();

	m_protocol.readBindParam(iActualNum,receiver,
			[this] (size_t index, std::string_view value, bool isBinary) {
				auto pParam = m_result.m_bindParamNodes[index];
				pParam->setBindParamMode(isBinary ? Operation::BINARY_PARAM : Operation::TEXT_PARAM);
				pParam->setString(value);
			});
	m_bBinded = true;

	resolve();
}

void WorkThreadInfo::handleExecute() {
	if (m_pPlan == nullptr) {
		PARSE_ERROR("No statement!");
	}
	size_t columnNum = m_pPlan->getResultColumns();

	m_pPlan->begin(m_bTerminate);

	while (m_pPlan->next(m_bTerminate)) {
		if (columnNum == 0)
			continue;

		if (!m_protocol.sendData(m_pPlan.get())) {
			m_protocol.flushSend(m_iAcceptFd);

			if (!m_protocol.sendData(m_pPlan.get())) {
				IO_ERROR("data row exceed send buffer length");
			}
		}
	} //while

	auto sInfo = m_pPlan->getInfoString();

	m_pPlan->end();
	DLOG(INFO)<< "Execute result:" << sInfo;

	if (m_bParsed) {
		m_protocol.sendShortMessage(m_iAcceptFd, '1');
		m_bParsed = false;
	}
	if (m_bBinded) {
		m_protocol.sendShortMessage(m_iAcceptFd, '2');
		m_bBinded = false;
	}
	m_protocol.sendShortMessage(m_iAcceptFd, 'C', sInfo);
	m_protocol.flushSend(m_iAcceptFd);
	m_pPlan = nullptr;

	//discard allocated string for bind param
	m_result.restore();
}

void WorkThreadInfo::run(int fd, const std::atomic_bool& bGlobalTerminate) {
	m_iAcceptFd = fd;

	m_bRunning = true;
	++m_iSessions;

	auto start = std::chrono::steady_clock::now();

	m_protocol.startup(fd);

	auto end = std::chrono::steady_clock::now();

	m_iClientTime += std::chrono::duration_cast<std::chrono::microseconds>(
			end - start).count();

	while (!bGlobalTerminate.load()) {
		char qtype = m_protocol.readMessageType(fd);
		if (qtype == 'X') {
			DLOG(INFO)<< "Client Terminate!";
			break;
		}
		PgDataReader receiver(m_protocol.readData(fd));

		start = std::chrono::steady_clock::now();
		auto handler = m_handler[qtype];
		if (handler == nullptr) {
			DLOG(INFO)<< "Unable to handler message " << qtype;
			break;
		}
		try {
			handler(receiver);
		} catch (ParseException& e) {
			m_protocol.clear();
			m_protocol.sendException(e, e.getStartPos());
			m_protocol.flushSend(fd);
			m_pPlan = nullptr;
		} catch (std::exception& e) {
			m_protocol.clear();
			m_protocol.sendException(e, -1);
			m_protocol.flushSend(fd);
			m_pPlan = nullptr;
		}

		if (qtype == 'Q') {
			// should also sync when handleQuery throws Exception
			m_protocol.sendSync(fd);
		}
		auto end = std::chrono::steady_clock::now();

		m_iClientTime += std::chrono::duration_cast<std::chrono::microseconds>(
				end - start).count();

	} //while

	m_bRunning = false;
}

