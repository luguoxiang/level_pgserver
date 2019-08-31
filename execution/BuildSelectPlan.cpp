#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/ExplainPlan.h"
#include "execution/UnionAllPlan.h"
#include "execution/GroupByPlan.h"
#include "execution/ReadFilePlan.h"


ExecutionPlanPtr buildPlanForOrderBy(const ParseNode* pNode, ExecutionPlanPtr& pPlan) {
	if (pNode == nullptr)
		return pPlan;

	assert(pNode->children() > 0);
	bool bAlreadySorted = true;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		assert(pChild->children() == 2);
		const ParseNode* pColumn = pChild->getChild(0);
		if(pColumn->m_type != NodeType::NAME) {
			PARSE_ERROR("Unsupported sort spec: ",pColumn->m_sExpr);
		}
		SortOrder order = (pChild->getChild(1)->m_op == Operation::ASC) ? SortOrder::Ascend: SortOrder::Descend;
		if (!pPlan->ensureSortOrder(i, pColumn->m_sValue, order)) {
			bAlreadySorted = false;
			break;
		}
	}
	if (bAlreadySorted) {
		return pPlan;
	}

	SortPlan* pSort = new SortPlan(pPlan);
	ExecutionPlanPtr pResult(pSort);

	assert(pNode->children() > 0);
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		const ParseNode* pColumn = pChild->getChild(0);

		bool bAscend = (pChild->getChild(1)->m_op == Operation::ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	}
	return pResult;
}

ExecutionPlanPtr buildPlanForProjection(const ParseNode* pNode, ExecutionPlanPtr& pPlan) {
	ProjectionPlan* pProjPlan = new ProjectionPlan(pPlan);
	ExecutionPlanPtr pResult(pProjPlan);

	assert(pNode && pNode->getChild(0));
	if (pNode->getChild(0)->m_type == NodeType::INFO
			&& pNode->getChild(0)->m_op == Operation::ALL_COLUMNS) {
		std::vector<std::string_view> columns;
		pPlan->getAllColumns(columns);
		if (columns.size() == 0) {
			PARSE_ERROR(
					"select * is not supported in current projection context");
		}
		for (auto& column : columns) {
			ParseNode node(NodeType::NAME,  Operation::NONE, column, 0, nullptr);
			node.m_sValue = column;
			pProjPlan->project(&node, column);
		}
		return pResult;
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
	return pResult;
}

ExecutionPlanPtr buildPlanForGroupBy(const ParseNode* pNode, ExecutionPlanPtr& pPlan) {
	if (pNode == nullptr)
		return pPlan;

	bool bNeedSort = false;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		if (pChild->m_type != NodeType::NAME) {
			PARSE_ERROR("Wrong group by clause!");
		}

		if (!pPlan->ensureSortOrder(i, pChild->m_sValue, SortOrder::Any)) {
			bNeedSort = true;
		}
	}
	ExecutionPlanPtr pResult = pPlan;
	if (bNeedSort) {
		auto pSort = new SortPlan(pPlan);
		pResult = ExecutionPlanPtr(pSort);
		for (size_t i = 0; i < pNode->children(); ++i) {
			auto pChild = pNode->getChild(i);
			assert(pChild);

			pSort->addSortSpecification(pChild, SortOrder::Any);
		}
	}
	auto pGroupBy = new GroupByPlan(pPlan);
	pResult = ExecutionPlanPtr(pGroupBy);
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		assert(pChild);

		pGroupBy->addGroupByColumn(pChild);
	}
	return pResult;
}

ExecutionPlanPtr buildPlanForLimit(const ParseNode* pNode, ExecutionPlanPtr& pPlan) {
	if (pNode == nullptr)
		return pPlan;


	LimitPlan* pLimitPlan = new LimitPlan(pPlan);
	ExecutionPlanPtr pResult(pLimitPlan);

	assert(pNode->children() == 2);

	const ParseNode* pCount = pNode->getChild(0);
	const ParseNode* pOffset = pNode->getChild(1);

	assert(pCount->m_type == NodeType::INT);
	assert(pOffset->m_type == NodeType::INT);
	int64_t iOffset = pOffset->m_iValue;
	int64_t iCount = pCount->m_iValue;
	pLimitPlan->setLimit(iCount, iOffset);
	return pResult;
}

struct PredicateAnalyzer {
	ParseResult& result = WorkThreadInfo::getParseResult();

	void collectOrOperators(const ParseNode* pPredicate,
			std::vector<const ParseNode*>& operators) {
		if (pPredicate->m_type != NodeType::OP) {
			PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
		}
		switch (pPredicate->m_op) {
		case Operation::AND: {
			assert(pPredicate->children() == 2);
			std::vector<const ParseNode*> left, right;

			collectOrOperators(pPredicate->getChild(0), left);
			collectOrOperators(pPredicate->getChild(1), right);

			if(left.size() == 1 && right.size() == 1) {
				operators.push_back(pPredicate);
			} else {
				for (size_t i = 0; i < left.size(); ++i) {
					for (size_t j = 0; j < right.size(); ++j) {
						auto pOp = result.newExprNode(Operation::AND, "", { left[i], right[j] });
						operators.push_back(pOp);
					}
				}
			}
			break;
		}
		case Operation::OR: {
			for (size_t i = 0; i < pPredicate->children(); ++i) {
				collectOrOperators(pPredicate->getChild(i), operators);
			}
			break;
		}
		default:
			if (pPredicate->children() == 2) {
				operators.push_back(pPredicate);
			} else {
				PARSE_ERROR("Unsupported query condition!");
			}
			break;
		}
	}
};

ExecutionPlanPtr buildPlanForFilter(const ParseNode* pNode, ExecutionPlanPtr& pPlan) {
	if (pNode == nullptr)
		return pPlan;

	FilterPlan* pFilter = new FilterPlan(pPlan);
	ExecutionPlanPtr pResult(pFilter);

	std::vector<const ParseNode*> operators;
	PredicateAnalyzer analyzer;
	analyzer.collectOrOperators(pNode, operators);

	for (auto pChild : operators) {
		pFilter->addPredicate(pChild);
	}
	return pResult;
}

void buildPlanForExplain(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 1);

	BUILD_PLAN(pNode->getChild(0));
	ExecutionPlanPtr pPlan = Tools::popPlan();
	assert(pPlan);
	ExplainPlan* pExplain = new ExplainPlan(pPlan);
	Tools::pushPlan(pExplain);
}

void buildPlanForUnionAll(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 2);
	BUILD_PLAN(pNode->getChild(0));
	BUILD_PLAN(pNode->getChild(1));
	ExecutionPlanPtr pRight = Tools::popPlan();
	ExecutionPlanPtr pLeft = Tools::popPlan();
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


void buildPlanForFileSelect(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 7);

	const ParseNode* pTable = pNode->getChild(SQL_SELECT_TABLE);
	assert(pTable && pTable->m_type == NodeType::NAME);
	const TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(
			pTable->m_sValue);
	if (pTableInfo == nullptr) {
		PARSE_ERROR("Table ", pTable->m_sValue," does not exist!");
	}

	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path"),
			pTableInfo->getAttribute("seperator", ","),
			Tools::case_equals(pTableInfo->getAttribute("ignore_first_line", "false"), "true"));

	ExecutionPlanPtr pResult(pValuePlan);

	std::vector<const DBColumnInfo*> columns;
	pTableInfo->getDBColumns(nullptr, columns);
	for (auto p:columns) {
		pValuePlan->addColumn(p);
	}
	pResult= buildPlanForFilter(pNode->getChild(SQL_SELECT_PREDICATE), pResult);
	pResult= buildPlanForGroupBy(pNode->getChild(SQL_SELECT_GROUPBY), pResult);
	pResult= buildPlanForFilter(pNode->getChild(SQL_SELECT_HAVING), pResult);
	pResult= buildPlanForOrderBy(pNode->getChild(SQL_SELECT_ORDERBY), pResult);
	pResult= buildPlanForLimit(pNode->getChild(SQL_SELECT_LIMIT), pResult);
	pResult= buildPlanForProjection(pNode->getChild(SQL_SELECT_PROJECT), pResult);

	Tools::pushPlan(pResult);
}

