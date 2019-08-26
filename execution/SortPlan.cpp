#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include "execution/WorkThreadInfo.h"

constexpr size_t SORT_BUFFER_SIZE = 64 * 1024 * 1024;

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		SingleChildPlan(PlanType::Sort, pPlan){
	assert(m_pPlan);
}

void SortPlan::begin() {
	m_pBuffer = std::make_unique<ExecutionBuffer>(SORT_BUFFER_SIZE);

	m_pPlan->begin();
	for (size_t i = 0; i < m_proj.size(); ++i) {
		m_types.push_back(getResultType(i));
	}

	while (m_pPlan->next()) {
		std::vector<ExecutionResult> results(m_proj.size());
		for (size_t i = 0; i < m_proj.size(); ++i) {
			int iSubIndex = m_proj[i].m_iSubIndex;
			m_pPlan->getResult(iSubIndex, &results[i]);
			assert(!results[i].isNull());
		}
		auto [row, size] = m_pBuffer->copyRow(results, m_types);

		m_rows.push_back(row);
	}
	m_pPlan->end();
	auto comp =
			[this](ExecutionBuffer::Row pRow1, ExecutionBuffer::Row pRow2) {
				for (size_t i = 0; i < m_sort.size(); ++i) {
					const SortSpec& spec = m_sort[i];
					assert(spec.m_iIndex < m_proj.size());

					int n = m_pBuffer->compare(pRow1, pRow2, spec.m_iIndex, m_types);
					if (n == 0) {
						continue;
					}

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

	//use stable sort so that first last aggregation function can work
	std::stable_sort(m_rows.begin(), m_rows.end(), comp);
	m_iCurrent = 0;
}

void SortPlan::end() {
	m_rows.clear();
	m_pBuffer.reset(nullptr);
}

bool SortPlan::next() {
	if (m_iCurrent >= m_rows.size())
		return false;
	++m_iCurrent;
	return true;
}

void SortPlan::getResult(size_t index, ExecutionResult* pInfo) {
	assert(m_iCurrent > 0);
	m_pBuffer->getResult(m_rows[m_iCurrent - 1], index, *pInfo, m_types);
}

int SortPlan::addProjection(const ParseNode* pNode)  {
	int index = m_pPlan->addProjection(pNode);
	if (index < 0)
	return index;

	for (size_t i = 0; i < m_proj.size(); ++i) {
		if (m_proj[i].m_iSubIndex == index)
		return i;
	}
	SortProjection proj;
	proj.m_iSubIndex = index;
	proj.m_sName = pNode->m_sExpr;
	m_proj.push_back(proj);
	return m_proj.size() - 1;
}

bool SortPlan::ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
		bool* pOrder) {
	if (m_sort.size() <= iSortIndex)
	return false;

	SortSpec& spec = m_sort[iSortIndex];

	if (sColumn != spec.m_sColumn)
	return false;

	if (pOrder == nullptr)
	return true;
	switch (spec.m_order) {
		case SortOrder::Ascend:
		return *pOrder;
		case SortOrder::Descend:
		return *pOrder == false;
		case SortOrder::Any:
		spec.m_order = *pOrder ? SortOrder::Ascend : SortOrder::Descend;
		return true;
	};
}

void SortPlan::addSortSpecification(const ParseNode* pNode, SortOrder order) {
	int i = addProjection(pNode);
	if (i < 0) {
		PARSE_ERROR("unrecognized column ", pNode->m_sExpr);
	}
	SortSpec spec;
	spec.m_iIndex = i;
	spec.m_sColumn = pNode->m_sExpr;
	spec.m_iSubIndex = m_proj[i].m_iSubIndex;
	spec.m_order = order;
	spec.m_type = m_pPlan->getResultType(spec.m_iSubIndex);
	m_sort.push_back(spec);
}
