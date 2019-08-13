#include "common/SqlParser.tab.h"
#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/ExplainPlan.h"
#include "execution/UnionAllPlan.h"
#include "execution/GroupByPlan.h"

void buildPlanForOrderBy(ParseNode* pNode) {
	if (pNode == nullptr)
		return;
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);

	assert(pNode->m_iChildNum > 0);
	bool bAlreadySorted = true;
	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pChild = pNode->m_children[i];
		assert(pChild);

		assert(pChild->m_iChildNum == 2);
		ParseNode* pColumn = pChild->m_children[0];
		bool bAscend = (OP_CODE(pChild->m_children[1]) == ASC);
		if (!pPlan->ensureSortOrder(i, pColumn->m_pszValue, &bAscend)) {
			bAlreadySorted = false;
			break;
		}
	}
	if (bAlreadySorted) {
		Tools::pushPlan(pPlan);
		return;
	}

	SortPlan* pSort = new SortPlan(pPlan);
	Tools::pushPlan(pSort);

	assert(pNode->m_iChildNum > 0);
	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pChild = pNode->m_children[i];
		ParseNode* pColumn = pChild->m_children[0];

		bool bAscend = (OP_CODE(pChild->m_children[1]) == ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	}
}

void buildPlanForProjection(ParseNode* pNode) {
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	ProjectionPlan* pProjPlan = new ProjectionPlan(pPlan);
	Tools::pushPlan(pProjPlan);

	assert(pNode && pNode->m_children[0]);
	if (pNode->m_children[0]->m_iType == NodeType::INFO
			&& pNode->m_children[0]->m_iValue == ALL_COLUMN) {
		std::vector<const char*> columns;
		pPlan->getAllColumns(columns);
		if (columns.size() == 0) {
			PARSE_ERROR(
					"select * is not supported in current projection context");
		}
		for (size_t i = 0; i < columns.size(); ++i) {
			ParseNode node;
			node.m_iType = NodeType::NAME;
			node.m_iChildNum = 0;
			node.m_pszValue = columns[i];
			node.m_pszExpr = columns[i];
			pProjPlan->project(&node, columns[i]);
		}
		return;
	}

	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pColumn = pNode->m_children[i];
		const char* pszAlias = nullptr;
		if (pColumn->m_iType == NodeType::OP && OP_CODE(pColumn) == AS) {
			assert(pColumn->m_iChildNum == 2);

			pszAlias = pColumn->m_children[1]->m_pszValue;

			pColumn = pColumn->m_children[0];
		}
		if (pszAlias == nullptr) {
			pszAlias = pColumn->m_pszExpr;
		}
		bool bOK = pProjPlan->project(pColumn, pszAlias);
		if (!bOK) {
			if (pColumn->m_iType != NodeType::FUNC) {
				PARSE_ERROR("unrecongnized column '%s'", pColumn->m_pszExpr);
			}
			pProjPlan->addGroupBy();
			bOK = pProjPlan->project(pColumn, pszAlias);
			if (!bOK) {
				PARSE_ERROR("unrecongnized column '%s'", pNode->m_pszExpr);
			}
		}
	}
}

void buildPlanForGroupBy(ParseNode* pNode) {
	if (pNode == 0)
		return;

	std::unique_ptr<ExecutionPlan> pChildPlan(Tools::popPlan());
	assert(pChildPlan.get());

	bool bNeedSort = false;
	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pChild = pNode->m_children[i];
		assert(pChild);

		if (pChild->m_iType != NodeType::NAME) {
			PARSE_ERROR("Wrong group by clause!");
		}

		const char* pszColumn = pChild->m_pszValue;
		if (!pChildPlan->ensureSortOrder(i, pszColumn, nullptr)) {
			bNeedSort = true;
		}
	}
	if (bNeedSort) {
		SortPlan* pSort = new SortPlan(pChildPlan.release());
		pChildPlan.reset(pSort);
		for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
			ParseNode* pChild = pNode->m_children[i];
			assert(pChild);

			pSort->addSortSpecification(pChild, SortOrder::Any);
		}
	}
	GroupByPlan* pPlan = new GroupByPlan(pChildPlan.release());
	Tools::pushPlan(pPlan);
	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pChild = pNode->m_children[i];
		assert(pChild);

		pPlan->addGroupByColumn(pChild);
	}
}

void buildPlanForLimit(ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	LimitPlan* pLimitPlan = new LimitPlan(pPlan);
	Tools::pushPlan(pLimitPlan);

	assert(pNode->m_iChildNum == 2);

	ParseNode* pCount = pNode->m_children[0];
	ParseNode* pOffset = pNode->m_children[1];

	assert(pCount->m_iType == NodeType::INT);
	assert(pOffset->m_iType == NodeType::INT);
	int64_t iOffset = pOffset->m_iValue;
	int64_t iCount = pCount->m_iValue;
	pLimitPlan->setLimit(iCount, iOffset);
}

static void parseQueryCondition(ParseNode* pPredicate, FilterPlan* pFilter) {
	if (pPredicate->m_iType != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate '%s'", pPredicate->m_pszExpr);
	}

	int iOpCode = OP_CODE(pPredicate);
	if (iOpCode == ANDOP) {
		for (int i = 0; i < pPredicate->m_iChildNum; ++i) {
			ParseNode* pChild = pPredicate->m_children[i];
			parseQueryCondition(pChild, pFilter);
		}
	} else if (pPredicate->m_iChildNum == 2) {
		pFilter->addPredicate(pPredicate);
	} else {
		PARSE_ERROR("Unsupported query condition!");
	}
}

void buildPlanForFilter(ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	FilterPlan* pFilter = new FilterPlan(pPlan);
	Tools::pushPlan(pFilter);

	parseQueryCondition(pNode, pFilter);
}

void buildPlanForExplain(ParseNode* pNode) {
	assert(pNode && pNode->m_iChildNum == 1);

	BUILD_PLAN(pNode->m_children[0]);
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	ExplainPlan* pExplain = new ExplainPlan(pPlan);
	Tools::pushPlan(pExplain);
}

void buildPlanForUnionAll(ParseNode* pNode) {
	assert(pNode && pNode->m_iChildNum == 2);
	BUILD_PLAN(pNode->m_children[0]);
	BUILD_PLAN(pNode->m_children[1]);
	ExecutionPlan* pRight = Tools::popPlan();
	ExecutionPlan* pLeft = Tools::popPlan();
	assert(pLeft && pRight);
	UnionAllPlan* pPlan = new UnionAllPlan(pLeft, pRight);
	Tools::pushPlan(pPlan);
	int count = pLeft->getResultColumns();
	if (count != pRight->getResultColumns()) {
		PARSE_ERROR(
				"left sub query's column number is not same with right one's!");
	}
	for (int i = 0; i < count; ++i) {
		DBDataType type1 = pLeft->getResultType(i);
		DBDataType type2 = pRight->getResultType(i);

		switch (type1) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			type1 = DBDataType::INT64;
			break;
		default:
			break;
		}

		switch (type2) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			type2 = DBDataType::INT64;
			break;
		default:
			break;
		}

		if (type1 != type2) {
			PARSE_ERROR("sub query column %d's type are not match");
		}
	}
}
