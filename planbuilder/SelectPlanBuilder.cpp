#include "SelectPlanBuilder.h"
#include "LevelDBPlanBuilder.h"
#include "BuildPlan.h"

#include "common/ParseNode.h"
#include "common/MetaConfig.h"
#include "common/ParseTools.h"

#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/GroupByPlan.h"
#include "execution/ReadFilePlan.h"


void SelectPlanBuilder::buildPlanForOrderBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	assert(pNode->children() > 0);
	bool bAlreadySorted = pNode->allChildOf([pPlan = m_pPlan.get()](size_t index, auto pChild) {
		assert(pChild);

		assert(pChild->children() == 2);
		const ParseNode* pColumn = pChild->getChild(0);

		SortOrder order =
				(pChild->getChild(1)->getOp() == Operation::ASC) ?
						SortOrder::Ascend : SortOrder::Descend;
		return pPlan->ensureSortOrder(index, pColumn->getString(), order);
	});

	if (bAlreadySorted) {
		return;
	}

	SortPlan* pSort = new SortPlan(m_pPlan.release());
	m_pPlan.reset(pSort);

	assert(pNode->children() > 0);

	pNode->forEachChild([pSort](size_t index, auto pChild) {
		assert(pChild);
		const ParseNode* pColumn = pChild->getChild(0);

		bool bAscend = (pChild->getChild(1)->getOp() == Operation::ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	});

}

void SelectPlanBuilder::buildPlanForProjection(const ParseNode* pNode) {
	assert(pNode);

	if (pNode->m_type == NodeType::INFO
			&& pNode->getOp() == Operation::ALL_COLUMNS) {

		std::vector<std::string_view> columns;
		m_pPlan->getAllColumns(columns);
		if (columns.size() == 0) {
			PARSE_ERROR(
					"select * is not supported in current projection context");
		}

		ProjectionPlan* pProjPlan = new ProjectionPlan(m_pPlan.release());
		m_pPlan.reset(pProjPlan);

		for (auto& column : columns) {
			ParseNode node(NodeType::NAME, Operation::NONE, column, 0, nullptr);
			node.setString(column);

			pProjPlan->project(&node, column);
		}
		return;
	}

	ProjectionPlan* pProjPlan = new ProjectionPlan(m_pPlan.release());
	m_pPlan.reset(pProjPlan);

	pNode->forEachChild([pProjPlan](size_t index, auto pColumn) {
		std::string_view sAlias;
		if (pColumn->m_type == NodeType::OP && pColumn->getOp() == Operation::AS) {
			assert(pColumn->children() == 2);

			sAlias = pColumn->getChild(1)->getString();

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
				PARSE_ERROR("Unrecognized column ", pColumn->m_sExpr);
			}
		}
	});

}

void SelectPlanBuilder::buildPlanForGroupBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	bool bNeedSort = pNode->anyChildOf([pPlan=m_pPlan.get()](size_t index, auto pChild) {
		assert(pChild);

		if (pChild->m_type != NodeType::NAME) {
			PARSE_ERROR("Wrong group by clause!");
		}
		return !pPlan->ensureSortOrder(index, pChild->getString(), SortOrder::Any);
	});

	if (bNeedSort) {
		auto pSort = new SortPlan(m_pPlan.release());
		m_pPlan.reset(pSort);
		pNode->forEachChild([pSort](size_t index, auto pChild) {
			pSort->addSortSpecification(pChild, SortOrder::Any);
		});
	}
	auto pGroupBy = new GroupByPlan(m_pPlan.release());
	m_pPlan.reset(pGroupBy);

	pNode->forEachChild([pGroupBy](size_t index, auto pChild) {
		pGroupBy->addGroupByColumn(pChild);
	});
}

namespace {
	int64_t nodeValueToInt(const ParseNode* pNode) {
		assert(pNode->m_type == NodeType::INT);
		int64_t iValue;
		auto sValue = pNode->getString();
		if(!absl::SimpleAtoi(sValue, &iValue)) {
			PARSE_ERROR("Could not convert to int:", sValue);
		}
		return iValue;
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

	int64_t iOffset = nodeValueToInt(pOffset);
	int64_t iCount = nodeValueToInt(pCount);
	pLimitPlan->setLimit(iCount, iOffset);
}

void SelectPlanBuilder::buildPlanForFilter(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	FilterPlan* pFilter = new FilterPlan(m_pPlan.release());
	m_pPlan.reset(pFilter);

	pFilter->setPredicate(pNode);
}
namespace {
	constexpr int SQL_SELECT_PROJECT = 0;
	constexpr int SQL_SELECT_TABLE = 1;
	constexpr int SQL_SELECT_PREDICATE = 2;
	constexpr int SQL_SELECT_GROUPBY = 3;
	constexpr int SQL_SELECT_HAVING = 4;
	constexpr int SQL_SELECT_ORDERBY = 5;
	constexpr int SQL_SELECT_LIMIT = 6;
}


void SelectPlanBuilder::buildPlanForReadFile(const TableInfo* pTableInfo) {
	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path"),
			pTableInfo->getAttribute("seperator", ","),
			absl::EqualsIgnoreCase(
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

	const ParseNode* pTable = pNode->getChild(SQL_SELECT_TABLE);
	assert(pTable);

	const ParseNode* pPredicate = pNode->getChild(SQL_SELECT_PREDICATE);
	if(pPredicate != nullptr) {
		if(pPredicate->isFalseConst()) {
			m_pPlan.reset(new EmptyPlan());
			return std::move(m_pPlan);
		} else if(pPredicate->isTrueConst()) {
			pPredicate = nullptr;
		}
	}
	if( pTable->m_type != NodeType::NAME ) {
		m_pPlan = buildPlan(pTable);
		buildPlanForFilter(pPredicate);
	}else{
		auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->getString());
		if (pTableInfo == nullptr) {
			PARSE_ERROR("table ", pTable->getString(), " not found");
		}
		if(pTableInfo->getKeyCount() > 0) {
			LevelDBPlanBuilder builder;
			m_pPlan = builder.buildSelectPlan(pTableInfo, pPredicate);
		} else {
			buildPlanForReadFile(pTableInfo);
			buildPlanForFilter(pPredicate);
		}

	}

	buildPlanForGroupBy(pNode->getChild(SQL_SELECT_GROUPBY));
	buildPlanForFilter(pNode->getChild(SQL_SELECT_HAVING));
	buildPlanForOrderBy(pNode->getChild(SQL_SELECT_ORDERBY));
	buildPlanForLimit(pNode->getChild(SQL_SELECT_LIMIT));
	buildPlanForProjection(pNode->getChild(SQL_SELECT_PROJECT));
	return std::move(m_pPlan);
}
