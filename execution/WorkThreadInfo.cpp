#include <cassert>
#include <limits>

#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "execution/ExecutionException.h"
#include "execution/ParseTools.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;


WorkThreadInfo::WorkThreadInfo(int fd, int port, int iIndex) :
		m_iListenFd(fd), m_port(port), m_iIndex(iIndex){

	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
	m_plans.clear();
}

void WorkThreadInfo::parse(const std::string_view sql) {
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



