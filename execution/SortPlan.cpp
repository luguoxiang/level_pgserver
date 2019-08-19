#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include "execution/WorkThreadInfo.h"

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		ExecutionPlan(PlanType::Sort), m_pPlan(pPlan), m_iCurrent(0) {
	assert(m_pPlan);
}

void SortPlan::begin() {
	if(m_proj.size() > 32) {
		throw new ExecutionException(ConcateToString("sort column number bigger than 32"));
	}
	ExecutionBuffer& buffer = WorkThreadInfo::m_pWorkThreadInfo->getExecutionBuffer();

	m_pPlan->begin();
	for (size_t i = 0; i < m_proj.size(); ++i) {
		m_types.push_back(getResultType(i));
	}

	while (m_pPlan->next()) {
		auto row = buffer.beginRow();
		for (size_t i = 0; i < m_proj.size(); ++i) {
			ExecutionResult result;
			int iSubIndex = m_proj[i].m_iSubIndex;
			m_pPlan->getResult(iSubIndex, &result);

			buffer.allocForColumn(m_types[i], result);
		}
		buffer.endRow();

		m_rows.push_back(row);
	}
	m_pPlan->end();
	auto comp =
			[this, &buffer](ExecutionBuffer::Row pRow1, ExecutionBuffer::Row pRow2) {
				for (size_t i = 0; i < m_sort.size(); ++i) {
					const SortSpec& spec = m_sort[i];
					assert(spec.m_iIndex < m_proj.size());

					int n = buffer.compare(pRow1, pRow2, spec.m_iIndex, m_types);
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

	std::stable_sort(m_rows.begin(), m_rows.end(), comp);
	m_iCurrent = 0;
}

void SortPlan::end() {
	m_rows.clear();
}

bool SortPlan::next() {
	if (m_iCurrent >= m_rows.size())
		return false;
	++m_iCurrent;
	return true;
}

void SortPlan::getResult(size_t index, ExecutionResult* pInfo) {
	assert(m_iCurrent > 0);
	ExecutionBuffer& buffer = WorkThreadInfo::m_pWorkThreadInfo->getExecutionBuffer();
	buffer.getResult(m_rows[m_iCurrent - 1], index, *pInfo, m_types);
}

int SortPlan::addProjection(ParseNode* pNode)  {
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

bool SortPlan::ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
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

void SortPlan::addSortSpecification(ParseNode* pNode, SortOrder order) {
	int i = addProjection(pNode);
	if (i < 0) {
		throw new ParseException(ConcateToString("unrecognized column ", pNode->m_sExpr));
	}
	SortSpec spec;
	spec.m_iIndex = i;
	spec.m_sColumn = pNode->m_sExpr;
	spec.m_iSubIndex = m_proj[i].m_iSubIndex;
	spec.m_order = order;
	spec.m_type = m_pPlan->getResultType(spec.m_iSubIndex);
	m_sort.push_back(spec);
}
