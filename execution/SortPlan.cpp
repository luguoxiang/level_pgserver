#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include "execution/WorkThreadInfo.h"

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		ExecutionPlan(PlanType::Sort), m_pPlan(pPlan), m_iCurrent(0) {
	assert(m_pPlan);
}

void SortPlan::begin() {
	m_pPlan->begin();
	while (m_pPlan->next()) {
		RowInfo* pRow= new RowInfo();
		pRow->reserve(m_proj.size());

		m_rows.push_back(pRow);
		for (size_t i = 0; i < m_proj.size(); ++i) {
			ResultInfo result;
			int iSubIndex = m_proj[i].m_iSubIndex;
			m_pPlan->getResult(iSubIndex, &result);
			pRow->push_back(result);
		}

	}
	m_pPlan->end();
	auto comp = [specList = m_sort, iColumnNum = m_proj.size()](RowInfo* pRow1, RowInfo* pRow2) {
		for (size_t i = 0; i < specList.size(); ++i) {
			const SortSpec& spec = specList[i];
			assert(spec.m_iIndex < iColumnNum);
			const ResultInfo& a = pRow1->at(spec.m_iIndex);
			const ResultInfo& b = pRow2->at(spec.m_iIndex);
			int n = a.compare(b, spec.m_type);
			if (n == 0)
				continue;

			switch (spec.m_order) {
			case SortOrder::Ascend:
			case SortOrder::Any:
				return n < 0;
			case SortOrder::Descend:
				return n > 0;
			default:
				assert(0);
				return false;
			};
		}
		return false; //equals is not less
	};

	std::stable_sort(m_rows.begin(), m_rows.end(), comp);
	m_iCurrent = 0;
}

void SortPlan::end() {
	for(auto p: m_rows) {
		delete p;
	}
	m_rows.clear();
}

bool SortPlan::next() {
	if (m_iCurrent >= m_rows.size())
		return false;
	++m_iCurrent;
	return true;
}

void SortPlan::getResult(size_t index, ResultInfo* pInfo) {
	assert(m_iCurrent > 0);
	*pInfo = m_rows[m_iCurrent - 1]->at(index);
}

