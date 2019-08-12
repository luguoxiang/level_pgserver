#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include "execution/WorkThreadInfo.h"

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		ExecutionPlan(PlanType::Sort), m_pPlan(pPlan), m_iCurrent(0) {
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
			case DBDataType::STRING:
			case DBDataType::BYTES:
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
	auto comp = [specList = m_sort, iColumnNum = m_proj.size()](ResultInfo* pRow1, ResultInfo* pRow2) {
		for (size_t i = 0; i < specList.size(); ++i) {
			const SortSpec& spec = specList[i];
			assert(spec.m_iIndex < iColumnNum);
			ResultInfo& a = pRow1[spec.m_iIndex];
			ResultInfo& b = pRow2[spec.m_iIndex];
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

