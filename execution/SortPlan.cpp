#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include "execution/WorkThreadInfo.h"

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		ExecutionPlan(Sort), m_pPlan(pPlan), m_iCurrent(0) {
	assert(m_pPlan);
}

void SortPlan::begin() {
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	m_pPlan->begin();
	while (m_pPlan->next()) {
		ResultInfo* pRow = new (*pInfo) ResultInfo[m_proj.size()];
		for (size_t i = 0; i < m_proj.size(); ++i) {
			ResultInfo* pResult = pRow + i;
			int iSubIndex = m_proj[i].m_iSubIndex;
			m_pPlan->getResult(iSubIndex, pResult);
			if (pResult->m_bNull)
				continue;

			switch (m_pPlan->getResultType(iSubIndex)) {
			case TYPE_STRING:
			case TYPE_BYTES:
				pResult->m_value.m_pszResult = pInfo->memdup(
						pResult->m_value.m_pszResult, pResult->m_len);
				break;
			default:
				break;
			};
		}
		m_rows.push_back(pRow);
	}
	m_pPlan->end();
	Compare comp(m_proj.size(), m_sort);
	std::stable_sort(m_rows.begin(), m_rows.end(), comp);
	m_iCurrent = 0;
}

void SortPlan::end() {
}

bool SortPlan::next() {
	if (m_iCurrent >= m_rows.size())
		return false;
	++m_iCurrent;
	return true;
}

void SortPlan::getResult(size_t index, ResultInfo* pInfo) {
	assert(m_iCurrent > 0);
	*pInfo = m_rows[m_iCurrent - 1][index];
}

bool SortPlan::Compare::operator()(ResultInfo* pRow1, ResultInfo* pRow2) {
	for (size_t i = 0; i < m_sort.size(); ++i) {
		const SortSpec& spec = m_sort[i];
		assert(spec.m_iIndex < m_iColumns);
		ResultInfo& a = pRow1[spec.m_iIndex];
		ResultInfo& b = pRow2[spec.m_iIndex];
		int n = a.compare(b, spec.m_type);
		if (n == 0)
			continue;

		switch (spec.m_order) {
		case Ascend:
		case Any:
			return n < 0;
		case Descend:
			return n > 0;
		default:
			assert(0);
			return false;
		};
	}
	return false; //equals is not less
}
