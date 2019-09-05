#include <cassert>
#include <limits>

#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "common/ParseTools.h"

#include "execution/ExecutionException.h"
#include "execution/BuildPlan.h"
#include "execution/BasePlan.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;


WorkThreadInfo::WorkThreadInfo(int iIndex) : m_iIndex(iIndex), m_rewritter(m_result){

	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
}



void WorkThreadInfo::setAcceptFd(int fd) {
	m_iAcceptFd = fd;
}
void WorkThreadInfo::cancel() {
	m_bTerminate.store(true);
	::close(m_iAcceptFd);
}
ExecutionPlanPtr WorkThreadInfo::resolve() {
	if (m_result.m_pResult == nullptr) {
		return ExecutionPlanPtr(new EmptyPlan());
	} else {
		return buildPlan(m_result.m_pResult);
	}
}
void WorkThreadInfo::parse(const std::string_view sql) {
	if (strncasecmp("DEALLOCATE", sql.data(), 10) == 0) {
		m_result.m_pResult = nullptr;
	} else if (strncasecmp("SET ", sql.data(), 4) == 0) {
		m_result.m_pResult = nullptr;
	} else {
		++m_iSqlCount;
		parseSql(&m_result, sql);

		m_result.m_pResult = m_rewritter.rewrite(m_result.m_pResult);
		if (m_result.m_pResult == nullptr) {
			throw new ParseException(&m_result);
		}
#ifndef NDEBUG
		print();
#endif
	}
}

void WorkThreadInfo::print() {
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}




