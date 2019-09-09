#include "execution/UnionAllPlan.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include <algorithm>



void UnionAllPlan::getResult(size_t index,  ExecutionResult& result, DBDataType type) {
	if(m_iCurrentIndex < m_plans.size()) {
		m_plans[m_iCurrentIndex]->getResult(index, result, type);
	} else {
		assert(0);
	}
}

void UnionAllPlan::begin() {
	if (m_order == SortOrder::Descend) {
		std::reverse(m_plans.begin(), m_plans.end());
	}
	m_plans[0]->begin();
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
		m_plans[m_iCurrentIndex]->begin();
	}
	++m_iCurrentRow;
	return true;
}

void UnionAllPlan::end() {
	for (auto& pPlan : m_plans) {
		pPlan->end();
	}
}

