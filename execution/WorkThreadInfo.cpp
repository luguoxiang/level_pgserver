#include <cassert>
#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "execution/ExecutionException.h"
#include "execution/ParseTools.h"
#include "common/MetaConfig.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;

WorkThreadInfo::WorkThreadInfo(int fd, int port, int iIndex) :
		m_iListenFd(fd), m_iAcceptFd(0), m_port(port), m_bRunning(false), m_iClientTime(
				0), m_iIndex(iIndex), m_iSessions(0), m_iExecScanTime(0), m_iBiggestExec(
				0), m_iSqlCount(0), m_pPlan(nullptr) {

	m_result = {};
	if (parseInit(&m_result)) {
		throw new ParseException("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
	m_plans.clear();
}

void WorkThreadInfo::parse(const std::string& sql) {
	parseSql(&m_result, sql);

	if (m_result.m_pResult == 0) {
		throw new ParseException(&m_result);
	}
}

void WorkThreadInfo::print() {
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}

ExecutionPlan* WorkThreadInfo::resolve() {
	assert(m_result.m_pResult);

	BUILD_PLAN(m_result.m_pResult);

	ExecutionPlan* pPlan = popPlan();
	assert(pPlan && m_plans.empty());
	return pPlan;
}


WorkerManager& WorkerManager::getInstance() {
	static WorkerManager manager;
	return manager;
}



