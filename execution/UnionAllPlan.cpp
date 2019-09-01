#include "execution/UnionAllPlan.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include "WorkThreadInfo.h"
#include <algorithm>



void UnionAllPlan::getResult(size_t index,  ExecutionResult& result) {
	if(m_iCurrentIndex < m_plans.size()) {
		m_plans[m_iCurrentIndex]->getResult(index, result);
	}
}

void UnionAllPlan::begin() {
	for (auto& pPlan : m_plans) {
		pPlan->begin();
	}
	m_iCurrentRow = 0;
	m_iCurrentIndex = 0;
}

bool UnionAllPlan::next() {
	if(m_iCurrentIndex >= m_plans.size()) {
		return false;
	}
	while (!m_plans[m_iCurrentIndex]->next()) {
		if(++m_iCurrentIndex >= m_plans.size()) {
			return false;
		}
	}
	++m_iCurrentRow;
	return true;
}

void UnionAllPlan::end() {
	for (auto& pPlan : m_plans) {
		pPlan->end();
	}
}

