#include <algorithm>

#include "common/MetaConfig.h"
#include "common/ParseException.h"
#include "common/ParseTools.h"

#include "execution/LevelDBDeletePlan.h"
#include "execution/BuildPlan.h"
#include "execution/SortPlan.h"
#include "execution/LimitPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ProjectionPlan.h"
#include "execution/GroupByPlan.h"
#include "execution/ReadFilePlan.h"
#include "execution/LevelDBScanPlan.h"
#include "execution/UnionAllPlan.h"

void LevelDBPlanBuilder::buildPlanForOrderBy(const ParseNode* pNode) {
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
				(pChild->getChild(1)->getOp() == Operation::ASC) ?
						SortOrder::Ascend : SortOrder::Descend;
		if (!m_pPlan->ensureSortOrder(i, pColumn->getString(), order)) {
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

		bool bAscend = (pChild->getChild(1)->getOp() == Operation::ASC);
		pSort->addSortSpecification(pColumn,
				bAscend ? SortOrder::Ascend : SortOrder::Descend);
	}
}

void LevelDBPlanBuilder::buildPlanForProjection(const ParseNode* pNode) {
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

	for (int i = 0; i < pNode->children(); ++i) {
		auto pColumn = pNode->getChild(i);
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
				PARSE_ERROR("Unrecognized column ", pNode->m_sExpr);
			}

		}
	}
}

void LevelDBPlanBuilder::buildPlanForGroupBy(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	bool bNeedSort = false;
	for (size_t i = 0; i < pNode->children(); ++i) {
		const ParseNode* pChild = pNode->getChild(i);
		assert(pChild);

		if (pChild->m_type != NodeType::NAME) {
			PARSE_ERROR("Wrong group by clause!");
		}

		if (!m_pPlan->ensureSortOrder(i, pChild->getString(), SortOrder::Any)) {
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

void LevelDBPlanBuilder::buildPlanForLimit(const ParseNode* pNode) {
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
	int64_t iOffset = pOffset->getInt();
	int64_t iCount = pCount->getInt();
	pLimitPlan->setLimit(iCount, iOffset);
}

void LevelDBPlanBuilder::buildPlanForFilter(const ParseNode* pNode) {
	if (pNode == nullptr)
		return;

	FilterPlan* pFilter = new FilterPlan(m_pPlan.release());
	m_pPlan.reset(pFilter);

	pFilter->setPredicate(pNode);
}

void LevelDBPlanBuilder::buildPlanForReadFile(const TableInfo* pTableInfo) {
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

struct ScanPlanInfo {
	ScanPlanInfo(const ParseNode* pNode, const TableInfo* pTableInfo);
	KeySearchRange* m_pRange;
	LevelDBScanPlan* m_pScan;
	ExecutionPlanPtr m_pPlan;
	std::set<std::string_view> m_solved;

	bool needFilter(const ParseNode* pNode);
	bool isFullScan() {
		return m_solved.empty();
	}
};

ScanPlanInfo::ScanPlanInfo(const ParseNode* pNode, const TableInfo* pTableInfo) {
	m_pScan = new LevelDBScanPlan(pTableInfo);
	m_pPlan.reset(m_pScan);

	m_pScan->setPredicate(pNode, m_solved);

	m_pRange = m_pScan->getKeySearchRange();
	assert(m_pRange);
	if (needFilter(pNode)) {
		auto pFilter = new FilterPlan(m_pPlan.release());
		m_pPlan.reset(pFilter);

		auto hasFilter = pFilter->addPredicate(pNode, &m_solved);
		assert(hasFilter);
	}

}

bool ScanPlanInfo::needFilter(const ParseNode* pNode) {
	assert(pNode->getOp() != Operation::OR);
	if (pNode->getOp() == Operation::AND) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			if (needFilter(pNode->getChild(i))) {
				return true;
			}
		}
		return false;
	}
	return m_solved.find(pNode->m_sExpr) == m_solved.end();
}


const ParseNode* LevelDBPlanBuilder::buildUnionAll(const TableInfo* pTableInfo, const ParseNode* pPredicate) {

	std::vector<ScanPlanInfo> rangeScanList;
	std::vector<ScanPlanInfo*> rangePtrList(pPredicate->children());
	rangeScanList.reserve(pPredicate->children());

	for (size_t i = 0; i < pPredicate->children(); ++i) {
		auto pChild = pPredicate->getChild(i);
		//should be rewritten
		assert(pChild->getOp() != Operation::OR);

		rangeScanList.emplace_back(pChild, pTableInfo);

		if (rangeScanList[i].isFullScan()) {
			//back to full scan plan
			m_pPlan.reset(new LevelDBScanPlan(pTableInfo));
			return pPredicate;
		}
		rangePtrList[i] = rangeScanList.data() + i;
	}

	std::sort(rangePtrList.begin(), rangePtrList.end(),
			[](ScanPlanInfo* pInfo1, ScanPlanInfo* pInfo2) {
				return pInfo1->m_pRange->compareStart(pInfo2->m_pRange) < 0;
	});

	for (size_t i = 1; i < rangePtrList.size(); ++i) {
		auto pInfo = rangePtrList[i];
		auto pLastInfo = rangePtrList[i - 1];

		if(pInfo->m_pRange->startAfterEnd(pLastInfo->m_pRange)) {
			continue;
		}

		//key scan ranges have overlap, back to full scan
		m_pPlan.reset(new LevelDBScanPlan(pTableInfo));
		return pPredicate;
	}
	UnionAllPlan* pUnion = new UnionAllPlan(true);
	m_pPlan.reset(pUnion);

	auto pDBIter = LevelDBHandler::getHandler(pTableInfo)->createIterator();

	for (size_t i = 0; i < rangePtrList.size(); ++i) {
		auto pScanInfo = rangePtrList[i];
		pScanInfo->m_pScan->setLevelDBIterator(pDBIter);
		pUnion->addChildPlan(pScanInfo->m_pPlan.release());
	}
	return nullptr;
}

const ParseNode* LevelDBPlanBuilder::buildPlanForLevelDB(const TableInfo* pTableInfo, const ParseNode* pPredicate) {
	if (pPredicate == nullptr) {
		m_pPlan.reset(new LevelDBScanPlan(pTableInfo));
	} else if (pPredicate->getOp() == Operation::OR) {
		return buildUnionAll(pTableInfo, pPredicate);
	} else {
		ScanPlanInfo info(pPredicate, pTableInfo);
		m_pPlan = std::move(info.m_pPlan);
	}
	return nullptr;
}
ExecutionPlanPtr LevelDBPlanBuilder::buildDeletePlan(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 2);

	const ParseNode* pTable = pNode->getChild(0);
	assert(pTable);

	const ParseNode* pPredicate = pNode->getChild(1);
	if(pPredicate != nullptr) {
		if(pPredicate->isFalseConst()) {
			m_pPlan.reset(new EmptyPlan());
			return std::move(m_pPlan);
		} else if(pPredicate->isTrueConst()) {
			pPredicate = nullptr;
		}
	}

	assert( pTable->m_type == NodeType::NAME );

	auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->getString());
	if (pTableInfo == nullptr) {
		PARSE_ERROR("table ", pTable->getString(), " not found");
	}
	if(pTableInfo->getKeyCount() > 0) {
		pPredicate = buildPlanForLevelDB(pTableInfo, pPredicate);
		buildPlanForFilter(pPredicate);
	} else {
		PARSE_ERROR("DELETE is not supported for table ", pTable->getString());
	}

	return ExecutionPlanPtr(new LevelDBDeletePlan(pTableInfo, m_pPlan.release()));
}

ExecutionPlanPtr LevelDBPlanBuilder::buildSelectPlan(const ParseNode* pNode) {
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
	}else{
		auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->getString());
		if (pTableInfo == nullptr) {
			PARSE_ERROR("table ", pTable->getString(), " not found");
		}
		if(pTableInfo->getKeyCount() > 0) {
			pPredicate = buildPlanForLevelDB(pTableInfo, pPredicate);
		} else {
			buildPlanForReadFile(pTableInfo);
		}
	}
	buildPlanForFilter(pPredicate);
	buildPlanForGroupBy(pNode->getChild(SQL_SELECT_GROUPBY));
	buildPlanForFilter(pNode->getChild(SQL_SELECT_HAVING));
	buildPlanForOrderBy(pNode->getChild(SQL_SELECT_ORDERBY));
	buildPlanForLimit(pNode->getChild(SQL_SELECT_LIMIT));
	buildPlanForProjection(pNode->getChild(SQL_SELECT_PROJECT));
	return std::move(m_pPlan);
}

