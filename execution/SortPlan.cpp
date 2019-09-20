#include "SortPlan.h"
#include <cassert>
#include <algorithm>
#include <sstream>

constexpr size_t SORT_BUFFER_SIZE = 64 * 1024 * 1024;

SortPlan::SortPlan(ExecutionPlan* pPlan) :
		SingleChildPlan(PlanType::Sort, pPlan){
	assert(m_pPlan);
}

void SortPlan::begin(const std::atomic_bool& bTerminated) {
	m_pBuffer.emplace(SORT_BUFFER_SIZE);

	m_pPlan->begin(bTerminated);
	for (size_t i = 0; i < m_proj.size(); ++i) {
		m_types.push_back(getResultType(i));
	}

	while (m_pPlan->next(bTerminated)) {
		std::vector<ExecutionResult> results(m_proj.size());
		for (size_t i = 0; i < m_proj.size(); ++i) {
			int iSubIndex = m_proj[i].m_iSubIndex;
			m_pPlan->getResult(iSubIndex, results[i], m_types[i]);
			assert(!results[i].isNull());
		}
		auto row = m_pBuffer->copyRow(results, m_types);

		m_rows.push_back(reinterpret_cast<const std::byte*>(row.data()));
	}
	m_pPlan->end();
	auto comp =
			[this](const std::byte* pRow1, const std::byte* pRow2) {
				for (size_t i = 0; i < m_sort.size(); ++i) {
					const SortSpec& spec = m_sort[i];
					assert(spec.m_iIndex < m_proj.size());
					DataRow row1(pRow1, m_types, 0);
					DataRow row2(pRow2, m_types, 0);
					int n = m_pBuffer->compare(row1, row2, spec.m_iIndex);
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

	std::sort(m_rows.begin(), m_rows.end(), comp);
	m_iCurrent = 0;
}

void SortPlan::end() {
	m_rows.clear();
	m_pBuffer.reset();
}

bool SortPlan::next(const std::atomic_bool& bTerminated) {
	CheckCancellation(bTerminated);
	if (m_iCurrent >= m_rows.size())
		return false;
	++m_iCurrent;
	return true;
}

void SortPlan::getResult(size_t index, ExecutionResult& result, DBDataType type) {
	assert(m_iCurrent > 0);
	DataRow row(m_rows[m_iCurrent - 1], m_types, 0);
	m_pBuffer->getResult(row, index, result);
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
		SortOrder order) {
	if (m_sort.size() <= iSortIndex) {
		return false;
	}

	SortSpec& spec = m_sort[iSortIndex];

	if (sColumn != spec.m_sColumn) {
		return false;
	}

	if (order == SortOrder::Any) {
		return true;
	}
	if(spec.m_order == SortOrder::Any) {
		spec.m_order = order;
	}
	return spec.m_order == order;
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
	m_sort.push_back(spec);
}
