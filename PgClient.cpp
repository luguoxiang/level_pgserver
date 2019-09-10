#include "PgClient.h"
#include <cassert>
#include <unistd.h>
#include <chrono>

#include <signal.h>
#include <sstream>
#include <vector>

#include <glog/logging.h>

#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ExecutionException.h"
#include "execution/WorkThreadInfo.h"

#include "planbuilder/BuildPlan.h"

PgClient::PgClient(WorkThreadInfo* pInfo, std::atomic_bool& bGlobalTerminate) :
		m_protocol(pInfo->getAcceptFd(), pInfo->getIndex()),
		m_bGlobalTerminate(bGlobalTerminate),
		m_pWorker(pInfo){
	assert(pInfo->getAcceptFd() >= 0);

	memset(m_handler, 0, sizeof(m_handler));

	m_handler['S'] = &PgClient::handleSync;

	m_handler['Q'] = &PgClient::handleQuery;

	m_handler['P'] = &PgClient::handleParse;
	m_handler['B'] = &PgClient::handleBind;
	m_handler['D'] = &PgClient::handleDescribe;
	m_handler['E'] = &PgClient::handleExecute;
}

void PgClient::resolve() {
	auto pTree = m_pWorker->getParseTree();
	if (pTree == nullptr) {
		m_pPlan.reset(new EmptyPlan());
	} else {
#ifndef NDEBUG
		printTree(pTree, 0);
#endif
		m_pPlan = buildPlan(pTree);
	}
}

void PgClient::handleSync() {
	m_protocol.sendSync();
}

void PgClient::handleQuery() {

	auto sql = m_protocol.readQueryInfo();

	m_pWorker->parse(sql);

	resolve();

	describeColumn();
	handleExecute();

	m_protocol.sendSync();
}

void PgClient::handleParse() {
	auto [sql, iParamNum] = m_protocol.readParseInfo();

	m_pWorker->parse(sql);

	if (iParamNum != m_pWorker->getBindParamNumber()) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iParamNum, ", actual ", m_pWorker->getBindParamNumber());
	}

	m_protocol.sendShortMessage('1');
}

void PgClient::handleBind() {
	m_pWorker->markParseBuffer();

	size_t iActualNum =m_pWorker->getBindParamNumber();

	m_protocol.readBindParam(iActualNum, [pWorker = m_pWorker] (size_t index, std::string_view value, bool isBinary) {
		auto pParam = pWorker->getBindParam(index);
		pParam->setBindParamMode(isBinary ? Operation::BINARY_PARAM : Operation::TEXT_PARAM);
		pParam->setString(value);
	});

	m_protocol.sendShortMessage('2');

	resolve();
	m_bDescribed = false;
}

void PgClient::handleDescribe() {
	m_protocol.readColumnDescribeInfo();

	describeColumn();

}

void PgClient::handleExecute() {
	if (m_pPlan == nullptr) {
		PARSE_ERROR("No statement!");
	}
	size_t columnNum = m_pPlan->getResultColumns();

	m_pPlan->begin();

	while (m_pPlan->next()) {
		if (columnNum == 0)
			continue;
		m_protocol.sendData(m_pPlan.get(), !m_bDescribed);
	} //while
	m_protocol.flush();

	auto sInfo = m_pPlan->getInfoString();

	m_pPlan->end();
	DLOG(INFO)<< "Execute result:" << sInfo;

	m_protocol.sendShortMessage('C', sInfo);

	m_pPlan = nullptr;

	//discard allocated string for bind param
	m_pWorker->restoreParseBuffer();
}

void PgClient::run() {
	auto start = std::chrono::steady_clock::now();

	m_protocol.startup();

	auto end = std::chrono::steady_clock::now();

	m_pWorker->m_iClientTime += std::chrono::duration_cast
			< std::chrono::microseconds > (end - start).count();

	while (!m_bGlobalTerminate.load()) {
		char qtype = m_protocol.readMessage();
		if (qtype == 'X') {
			DLOG(INFO)<< "Client Terminate!";
			break;
		}
		DLOG(INFO)<<qtype;
		start = std::chrono::steady_clock::now();
		MessageHandler handler = m_handler[qtype];
		if (handler == nullptr) {
			char s[] = {qtype, '\0' };
			IO_ERROR("Unable to handler message ", s);
		}

		try {
			(this->*handler)();
		} catch (Exception* pe) {
			m_protocol.sendException(pe);
			m_protocol.flush();
			m_pPlan = nullptr;
		}

		auto end = std::chrono::steady_clock::now();

		m_pWorker->m_iClientTime += std::chrono::duration_cast
				< std::chrono::microseconds > (end - start).count();

	} //while
}


void PgClient::describeColumn() {
	m_bDescribed = true;

	m_protocol.sendColumnDescription(m_pPlan.get());
}
