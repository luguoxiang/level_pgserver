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


WorkThreadInfo::WorkThreadInfo(int port, int iIndex) :
		m_port(port), m_iIndex(iIndex), m_rewritter(m_result){

	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
}

void WorkThreadInfo::clearPlan() {
	std::lock_guard < std::mutex > lock(m_mutex);
	m_pPlan = nullptr;
}

void WorkThreadInfo::cancel() {
	std::lock_guard < std::mutex > lock(m_mutex);

	auto pPlan = m_pPlan.get();
	if (pPlan != nullptr){
		LOG(INFO)<< "cancel running execution plan ...";
		pPlan->cancel();
	}
}
void WorkThreadInfo::resolve() {
	std::lock_guard < std::mutex > lock(m_mutex);
	if (m_result.m_pResult == nullptr) {
		m_pPlan.reset(new EmptyPlan());
	} else {
		m_pPlan = buildPlan(m_result.m_pResult);
	}
}
void WorkThreadInfo::parse(const std::string_view sql) {
	if (strncasecmp("DEALLOCATE", sql.data(), 10) == 0) {
		m_result.m_pResult = nullptr;
	} else if (strncasecmp("SET ", sql.data(), 4) == 0) {
		m_result.m_pResult = nullptr;
	} else {
		m_pPlan = nullptr;
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




