#include <cassert>
#include <limits>

#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "execution/ExecutionException.h"
#include "execution/ParseTools.h"
#include "execution/BuildPlan.h"
#include "execution/BasePlan.h"

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
}

void WorkThreadInfo::resolve(const std::string_view sql) {
	if (strncasecmp("DEALLOCATE", sql.data(), 10) == 0) {
		m_pPlan.reset(new LeafPlan(PlanType::Other));
		DLOG(INFO) << sql;
	} else if (strncasecmp("SET ", sql.data(), 4) == 0) {
		m_pPlan.reset(new LeafPlan(PlanType::Other));
		DLOG(INFO) << sql;
	} else {
		m_pPlan = nullptr;
		parseSql(&m_result, sql);

		if (m_result.m_pResult == nullptr) {
			throw new ParseException(&m_result);
		}

		m_pPlan = buildPlan(m_result.m_pResult);
	}
}

void WorkThreadInfo::print() {
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}


WorkerManager& WorkerManager::getInstance() {
	static WorkerManager manager;
	return manager;
}



