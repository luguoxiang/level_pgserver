#include <algorithm>

#include "common/MetaConfig.h"
#include "common/ParseException.h"
#include "execution/BuildPlan.h"
#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/GroupByPlan.h"
#include "execution/ReadFilePlan.h"
#include "execution/LevelDBScanPlan.h"
#include "execution/UnionAllPlan.h"

void SelectPlanBuilder::buildPlanForOrderBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	assert(pNode->children() > 0);
	bool bAlreadySorted = true;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		assert(pChild->children() == 2);
		const ParseNode* pColumn = pChild->getChild(0);
		if (pColumn->m_type != NodeType::NAME) {
			PARSE_ERROR("Unsupported sort spec: ", pColumn->m_sExpr);
		}
		SortOrder order =
				(pChild->getChild(1)->m_op == Operation::ASC) ?
						SortOrder::Ascend : SortOrder::Descend;
		if (!m_pPlan->ensureSortOrder(i, pColumn->m_sValue, order)) {
			bAlreadySorted = false;
			break;
		}
	}
	if (bAlreadySorted) {
		return;
	}

	SortPlan* pSort = new SortPlan(m_pPlan.release());
	m_pPlan.reset(pSort);

	assert(pNode->children() > 0);
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		const ParseNode* pColumn = pChild->getChild(0);

		bool bAscend = (pChild->getChild(1)->m_op == Operation::ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	}
}

void SelectPlanBuilder::buildPlanForProjection(const ParseNode* pNode) {
	ProjectionPlan* pProjPlan = new ProjectionPlan(m_pPlan.release());
	m_pPlan.reset(pProjPlan);

	assert(pNode && pNode->getChild(0));
	if (pNode->getChild(0)->m_type == NodeType::INFO
			&& pNode->getChild(0)->m_op == Operation::ALL_COLUMNS) {
		std::vector<std::string_view> columns;
		m_pPlan->getAllColumns(columns);
		if (columns.size() == 0) {
			PARSE_ERROR(
					"select * is not supported in current projection context");
		}
		for (auto& column : columns) {
			ParseNode node(NodeType::NAME, Operation::NONE, column, 0, nullptr);
			node.m_sValue = column;
			pProjPlan->project(&node, column);
		}
		return;
	}
	for (int i = 0; i < pNode->children(); ++i) {
		auto pColumn = pNode->getChild(i);
		std::string_view sAlias;
		if (pColumn->m_type == NodeType::OP && pColumn->m_op == Operation::AS) {
			assert(pColumn->children() == 2);

			sAlias = pColumn->getChild(1)->m_sValue;

			pColumn = pColumn->getChild(0);
		}
		if (sAlias == "") {
			sAlias = pColumn->m_sExpr;
		}
		bool bOK = pProjPlan->project(pColumn, sAlias);
		if (!bOK) {
			if (pColumn->m_type != NodeType::FUNC) {
				PARSE_ERROR("Unrecognized column ", pColumn->m_sExpr);
			}

			if (!pProjPlan->addGroupBy()
					|| !pProjPlan->project(pColumn, sAlias)) {
				PARSE_ERROR("Unrecognized column ", pNode->m_sExpr);
			}

		}
	}
}

void SelectPlanBuilder::buildPlanForGroupBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	bool bNeedSort = false;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		if (pChild->m_type != NodeType::NAME) {
			PARSE_ERROR("Wrong group by clause!");
		}

		if (!m_pPlan->ensureSortOrder(i, pChild->m_sValue, SortOrder::Any)) {
			bNeedSort = true;
		}
	}

	if (bNeedSort) {
		auto pSort = new SortPlan(m_pPlan.release());
		m_pPlan.reset(pSort);
		for (size_t i = 0; i < pNode->children(); ++i) {
			auto pChild = pNode->getChild(i);
			assert(pChild);

			pSort->addSortSpecification(pChild, SortOrder::Any);
		}
	}
	auto pGroupBy = new GroupByPlan(m_pPlan.release());
	m_pPlan.reset(pGroupBy);
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		assert(pChild);

		pGroupBy->addGroupByColumn(pChild);
	}
}

void SelectPlanBuilder::buildPlanForLimit(const ParseNode* pNode) {
	if (pNode == nullptr) {
		return;
	}

	LimitPlan* pLimitPlan = new LimitPlan(m_pPlan.release());
	m_pPlan.reset(pLimitPlan);

	assert(pNode->children() == 2);

	const ParseNode* pCount = pNode->getChild(0);
	const ParseNode* pOffset = pNode->getChild(1);

	assert(pCount->m_type == NodeType::INT);
	assert(pOffset->m_type == NodeType::INT);
	int64_t iOffset = pOffset->m_iValue;
	int64_t iCount = pCount->m_iValue;
	pLimitPlan->setLimit(iCount, iOffset);
}

void SelectPlanBuilder::buildPlanForFilter(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	FilterPlan* pFilter = new FilterPlan(m_pPlan.release());
	m_pPlan.reset(pFilter);

	if (pNode->m_op == Operation::OR) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			auto pChild = pNode->getChild(i);
			//should be rewritten
			assert(pChild->m_op != Operation::OR);
			pFilter->addPredicate(pChild);
		}
	} else {
		pFilter->addPredicate(pNode);
	}
}

SelectPlanBuilder::SelectPlanBuilder(const TableInfo* pTableInfo) {
	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path"),
			pTableInfo->getAttribute("seperator", ","),
			Tools::case_equals(
					pTableInfo->getAttribute("ignore_first_line", "false"),
					"true"));

	m_pPlan.reset(pValuePlan);

	std::vector<const DBColumnInfo*> columns;
	pTableInfo->getDBColumns(nullptr, columns);
	for (auto p : columns) {
		pValuePlan->addColumn(p);
	}
}

ExecutionPlanPtr SelectPlanBuilder::build(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 7);

	buildPlanForFilter(pNode->getChild(SQL_SELECT_PREDICATE));
	buildPlanForGroupBy(pNode->getChild(SQL_SELECT_GROUPBY));
	buildPlanForFilter(pNode->getChild(SQL_SELECT_HAVING));
	buildPlanForOrderBy(pNode->getChild(SQL_SELECT_ORDERBY));
	buildPlanForLimit(pNode->getChild(SQL_SELECT_LIMIT));
	buildPlanForProjection(pNode->getChild(SQL_SELECT_PROJECT));
	return ExecutionPlanPtr(m_pPlan.release());
}

struct ScanPlanInfo {
	void build(const ParseNode* pNode, const TableInfo* pTableInfo);
	LevelDBScanPlan* m_pScan = nullptr;
	ExecutionPlanPtr m_pPlan;
	std::set<std::string_view> m_solved;

	bool needFilter(const ParseNode* pNode);
	bool isFullScan() {
		return m_solved.empty();
	}
};

void ScanPlanInfo::build(const ParseNode* pNode, const TableInfo* pTableInfo) {
	m_pScan = new LevelDBScanPlan(pTableInfo);
	m_pPlan.reset(m_pScan);

	m_pScan->setPredicate(pNode, m_solved);

	if (needFilter(pNode)) {
		auto pFilter = new FilterPlan(m_pPlan.release());
		m_pPlan.reset(pFilter);

		auto hasFilter = pFilter->addPredicate(pNode, &m_solved);
		assert(hasFilter);
	}

}

bool ScanPlanInfo::needFilter(const ParseNode* pNode) {
	if (pNode->m_op == Operation::AND) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			if (needFilter(pNode->getChild(i))) {
				return true;
			}
		}
		return false;
	}
	return m_solved.find(pNode->m_sExpr) == m_solved.end();
}
void LevelDBSelectPlanBuilder::buildFullScan(const ParseNode* pNode) {
	auto pFilter = new FilterPlan(new LevelDBScanPlan(m_pTableInfo));
	m_pPlan.reset(pFilter);
	pFilter->addPredicate(pNode);
}

void LevelDBSelectPlanBuilder::buildUnionAll(const ParseNode* pNode) {

	std::vector<ScanPlanInfo> rangeScanList(pNode->children());
	std::vector<ScanPlanInfo*> rangePtrList(pNode->children());
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		//should be rewritten
		assert(pChild->m_op != Operation::OR);

		auto& scanInfo = rangeScanList[i];
		scanInfo.build(pChild, m_pTableInfo);

		if (scanInfo.isFullScan()) {
			//back to full scan plan
			buildFullScan(pNode);
			return;
		}
		rangePtrList[i] = rangeScanList.data() + i;
	}

	std::sort(rangePtrList.begin(), rangePtrList.end(),
			[](ScanPlanInfo* pRange1, ScanPlanInfo* pRange2) {
				auto& start1 = pRange1->m_pScan->getStartRow();
				auto& start2 = pRange2->m_pScan->getStartRow();
				return start1.compare(start2);
	});

	for (size_t i = 1; i < pNode->children(); ++i) {
		auto pInfo = rangePtrList[i];
		auto pLastInfo = rangePtrList[i - 1];

		auto& start = pInfo->m_pScan->getStartRow();
		bool startInclusive = pInfo->m_pScan->startInclusive();

		auto& lastEnd = pLastInfo->m_pScan->getEndRow();
		bool lastEndInclusive = pLastInfo->m_pScan->endInclusive();

		int n = start.compare(lastEnd);
		if (n > 0) {
			continue;
		} else if(n == 0 && (!lastEndInclusive || !startInclusive)) {
			continue;
		}

		//key scan ranges have overlap, back to full scan
		buildFullScan(pNode);
		return;
	}
	UnionAllPlan* pUnion = new UnionAllPlan(true);
	m_pPlan.reset(pUnion);
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pScanInfo = rangePtrList[i];
		pUnion->addChildPlan(pScanInfo->m_pPlan.release());
	}
}

ExecutionPlanPtr LevelDBSelectPlanBuilder::build(const ParseNode* pNode) {
	auto pPredicate = pNode->getChild(SQL_SELECT_PREDICATE);
	if (pPredicate == nullptr) {
		m_pPlan.reset(new LevelDBScanPlan(m_pTableInfo));
	} else if (pPredicate->m_op == Operation::OR) {
		buildUnionAll(pNode);
	} else {
		ScanPlanInfo info;
		info.build(pNode, m_pTableInfo);
		m_pPlan.reset(info.m_pPlan.release());

	}
	buildPlanForGroupBy(pNode->getChild(SQL_SELECT_GROUPBY));
	buildPlanForFilter(pNode->getChild(SQL_SELECT_HAVING));
	buildPlanForOrderBy(pNode->getChild(SQL_SELECT_ORDERBY));
	buildPlanForLimit(pNode->getChild(SQL_SELECT_LIMIT));
	buildPlanForProjection(pNode->getChild(SQL_SELECT_PROJECT));
	return ExecutionPlanPtr(m_pPlan.release());

}
