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

WorkThreadInfo::WorkThreadInfo(int iIndex, const std::atomic_bool& bGlobalTerminate)
		: m_iIndex(iIndex)
		, m_rewritter(m_result)
		, m_bGlobalTerminate(bGlobalTerminate){
	m_bTerminate.store(false);
	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}

	m_handler['S'] = [this] () {
		m_protocol->sendSync();
	};

	m_handler['Q'] = std::bind(&WorkThreadInfo::handleQuery, this);

	m_handler['P'] = std::bind(&WorkThreadInfo::handleParse, this);
	m_handler['B'] = std::bind(&WorkThreadInfo::handleBind, this);

	m_handler['D'] = [this] () {
			m_protocol->readColumnDescribeInfo();
			m_protocol->sendColumnDescription(m_pPlan.get());
			m_protocol->flush();
	};

	m_handler['E'] = std::bind(&WorkThreadInfo::handleExecute, this);
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
}

void WorkThreadInfo::cancel(bool planOnly) {
	LOG(INFO) << "Cancel worker" << m_iIndex;
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
		LOG(WARNING) << "Transaction is not supported";
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
		if(pTree->m_type != NodeType::PLAN) {
			PARSE_ERROR("WRONG NODE ", pTree->m_sExpr);
		}
		if(pTree->getOp() ==  Operation::WORKLOAD) {
			m_pPlan = ExecutionPlanPtr(new WorkloadResult());
		} else {
			m_pPlan = buildPlan(pTree);
		}
	}
}


void WorkThreadInfo::handleQuery() {

	auto sql = m_protocol->readQueryInfo();

	parse(sql);

	resolve();

	m_protocol->sendColumnDescription(m_pPlan.get());

	m_protocol->flush();

	handleExecute();
}

void WorkThreadInfo::handleParse() {
	auto [sql, iParamNum] = m_protocol->readParseInfo();

	parse(sql);

	if (iParamNum != getBindParamNumber()) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iParamNum, ", actual ", m_pWorker->getBindParamNumber());
	}

	m_protocol->sendShortMessage('1');
}

void WorkThreadInfo::handleBind() {
	m_result.mark();

	size_t iActualNum = getBindParamNumber();

	m_protocol->readBindParam(iActualNum, [this] (size_t index, std::string_view value, bool isBinary) {
		auto pParam = getBindParam(index);
		pParam->setBindParamMode(isBinary ? Operation::BINARY_PARAM : Operation::TEXT_PARAM);
		pParam->setString(value);
	});

	m_protocol->sendShortMessage('2');

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
		m_protocol->sendData(m_pPlan.get());
	} //while
	m_protocol->flush();

	auto sInfo = m_pPlan->getInfoString();

	m_pPlan->end();
	DLOG(INFO)<< "Execute result:" << sInfo;

	m_protocol->sendShortMessage('C', sInfo);

	m_pPlan = nullptr;

	//discard allocated string for bind param
	m_result.restore();
}

void WorkThreadInfo::run(int fd) {
	m_iAcceptFd = fd;

	m_bRunning = true;
	++m_iSessions;

	m_protocol.emplace(fd, m_iIndex);

	auto start = std::chrono::steady_clock::now();

	m_protocol->startup();

	auto end = std::chrono::steady_clock::now();

	m_iClientTime += std::chrono::duration_cast
			< std::chrono::microseconds > (end - start).count();

	while (!m_bGlobalTerminate.load()) {
		char qtype = m_protocol->readMessage();
		if (qtype == 'X') {
			DLOG(INFO)<< "Client Terminate!";
			break;
		}

		start = std::chrono::steady_clock::now();
		auto handler = m_handler[qtype];
		if (handler == nullptr) {
			DLOG(INFO)<< "Unable to handler message " << qtype;
			break;
		}

		try {
			handler();
		} catch (ParseException& e) {
			m_protocol->sendParseException(e);
			m_protocol->flush();
			m_pPlan = nullptr;
		} catch (std::exception& e) {
			m_protocol->sendException(e);
			m_protocol->flush();
			m_pPlan = nullptr;
		}

		if(qtype == 'Q') {
			// should also sync when handleQuery throws Exception
			m_protocol->sendSync();
		}
		auto end = std::chrono::steady_clock::now();

		m_iClientTime += std::chrono::duration_cast
				< std::chrono::microseconds > (end - start).count();

	} //while
	m_protocol.reset();

	m_bRunning = false;

	m_iAcceptFd = 0;
}







