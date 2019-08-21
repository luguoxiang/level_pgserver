#include "common/SqlParser.tab.h"
#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/ExplainPlan.h"
#include "execution/UnionAllPlan.h"
#include "execution/GroupByPlan.h"

void buildPlanForOrderBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);

	assert(pNode->children() > 0);
	bool bAlreadySorted = true;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		assert(pChild->children() == 2);
		const ParseNode* pColumn = pChild->getChild(0);
		bool bAscend = (OP_CODE(pChild->getChild(1)) == ASC);
		if (!pPlan->ensureSortOrder(i, pColumn->m_sValue, &bAscend)) {
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

	assert(pNode->children() > 0);
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		const ParseNode* pColumn = pChild->getChild(0);

		bool bAscend = (OP_CODE(pChild->getChild(1)) == ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	}
}

void buildPlanForProjection(const ParseNode* pNode) {
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	ProjectionPlan* pProjPlan = new ProjectionPlan(pPlan);
	Tools::pushPlan(pProjPlan);

	assert(pNode && pNode->getChild(0));
	if (pNode->getChild(0)->m_type == NodeType::INFO
			&& pNode->getChild(0)->m_iValue == ALL_COLUMN) {
		std::vector<std::string_view> columns;
		pPlan->getAllColumns(columns);
		if (columns.size() == 0) {
			throw new ParseException(
					"select * is not supported in current projection context");
		}
		for (auto& column: columns) {
			ParseNode node(NodeType::NAME, column, 0, nullptr);
			node.m_sValue = column;
			pProjPlan->project(&node, column);
		}
		return;
	}
	for(int i =0 ;i< pNode->children(); ++i) {
		auto pColumn = pNode->getChild(i);
		std::string_view sAlias;
		if (pColumn->m_type == NodeType::OP && OP_CODE(pColumn) == AS) {
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
					throw new ParseException(ConcateToString("Unrecognized column ", pColumn->m_sExpr));
			}

			if (!pProjPlan->addGroupBy() || !pProjPlan->project(pColumn, sAlias)) {
				throw new ParseException(ConcateToString("Unrecognized column ", pNode->m_sExpr));
			}

		}
	}
}

void buildPlanForGroupBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	std::unique_ptr<ExecutionPlan> pChildPlan(Tools::popPlan());
	assert(pChildPlan.get());

	bool bNeedSort = false;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		if (pChild->m_type != NodeType::NAME) {
			throw new ParseException("Wrong group by clause!");
		}

		if (!pChildPlan->ensureSortOrder(i, pChild->m_sValue, nullptr)) {
			bNeedSort = true;
		}
	}
	if (bNeedSort) {
		SortPlan* pSort = new SortPlan(pChildPlan.release());
		pChildPlan.reset(pSort);
		for(size_t i=0;i<pNode->children(); ++i) {
			auto pChild = pNode->getChild(i);
			assert(pChild);

			pSort->addSortSpecification(pChild, SortOrder::Any);
		}
	}
	GroupByPlan* pPlan = new GroupByPlan(pChildPlan.release());
	Tools::pushPlan(pPlan);
	for(size_t i=0;i<pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		assert(pChild);

		pPlan->addGroupByColumn(pChild);
	}
}

void buildPlanForLimit(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	LimitPlan* pLimitPlan = new LimitPlan(pPlan);
	Tools::pushPlan(pLimitPlan);

	assert(pNode->children() == 2);

	const ParseNode* pCount = pNode->getChild(0);
	const ParseNode* pOffset = pNode->getChild(1);

	assert(pCount->m_type == NodeType::INT);
	assert(pOffset->m_type == NodeType::INT);
	int64_t iOffset = pOffset->m_iValue;
	int64_t iCount = pCount->m_iValue;
	pLimitPlan->setLimit(iCount, iOffset);
}

static void parseQueryCondition(const ParseNode* pPredicate, FilterPlan* pFilter) {
	if (pPredicate->m_type != NodeType::OP) {
		throw new ParseException(ConcateToString("Unsupported predicate " , pPredicate->m_sExpr));
	}

	if (OP_CODE(pPredicate) == ANDOP) {
		for (int i = 0; i < pPredicate->children(); ++i) {
			const ParseNode* pChild = pPredicate->getChild(i);
			parseQueryCondition(pChild, pFilter);
		}
	} else if (pPredicate->children() == 2) {
		pFilter->addPredicate(pPredicate);
	} else {
		throw new ParseException("Unsupported query condition!");
	}
}

void buildPlanForFilter(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	FilterPlan* pFilter = new FilterPlan(pPlan);
	Tools::pushPlan(pFilter);

	parseQueryCondition(pNode, pFilter);
}

void buildPlanForExplain(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 1);

	BUILD_PLAN(pNode->getChild(0));
	ExecutionPlan* pPlan = Tools::popPlan();
	assert(pPlan);
	ExplainPlan* pExplain = new ExplainPlan(pPlan);
	Tools::pushPlan(pExplain);
}

void buildPlanForUnionAll(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 2);
	BUILD_PLAN(pNode->getChild(0));
	BUILD_PLAN(pNode->getChild(1));
	ExecutionPlan* pRight = Tools::popPlan();
	ExecutionPlan* pLeft = Tools::popPlan();
	assert(pLeft && pRight);
	UnionAllPlan* pPlan = new UnionAllPlan(pLeft, pRight);
	Tools::pushPlan(pPlan);
	int count = pLeft->getResultColumns();
	if (count != pRight->getResultColumns()) {
		throw new ParseException(
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
			throw new ParseException("sub query column %d's type are not match");
		}
	}
}
