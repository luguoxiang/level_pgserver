#include <algorithm>

#include "LevelDBPlanBuilder.h"
#include "BuildPlan.h"

#include "common/MetaConfig.h"
#include "common/ParseException.h"
#include "common/ParseTools.h"

#include "execution/LevelDBDeletePlan.h"
#include "execution/FilterPlan.h"
#include "execution/LevelDBScanPlan.h"
#include "execution/LevelDBInsertPlan.h"
#include "execution/UnionAllPlan.h"

namespace {

class ScanPlanInfo {
public:
	ScanPlanInfo(const ParseNode* pNode, const TableInfo* pTableInfo);
	LevelDBScanPlan* m_pScan;
	ExecutionPlanPtr m_pPlan;

	bool needFilter(const ParseNode* pNode);
	bool isFullScan() {
		return m_solved.empty();
	}
private:
	std::set<std::string_view> m_solved;
};

ScanPlanInfo::ScanPlanInfo(const ParseNode* pPredicate, const TableInfo* pTableInfo) {
	m_pScan = new LevelDBScanPlan(pTableInfo);
	m_pPlan.reset(m_pScan);

	if(pPredicate  == nullptr) {
		return;
	}
	assert(pPredicate->m_type == NodeType::OP);

	if(pPredicate->getOp() == Operation::OR) {
		auto pFilter = new FilterPlan(m_pPlan.release());
		m_pPlan.reset(pFilter);

		pPredicate->forEachChild([pFilter](size_t index, auto pChild) {
			pFilter->addPredicate(pChild, nullptr);
		});
		return;
	}

	m_pScan->setPredicate(pPredicate, m_solved);

	if (needFilter(pPredicate)) {
		auto pFilter = new FilterPlan(m_pPlan.release());
		m_pPlan.reset(pFilter);

		auto hasFilter = pFilter->addPredicate(pPredicate, &m_solved);
		assert(hasFilter);
	}

}

bool ScanPlanInfo::needFilter(const ParseNode* pNode) {
	assert(pNode->getOp() != Operation::OR);
	if (pNode->getOp() == Operation::AND) {
		return pNode->anyChildOf([this](size_t index, auto pChild) {
			return needFilter(pChild);
		});
	}
	return m_solved.find(pNode->m_sExpr) == m_solved.end();
}

}

ExecutionPlanPtr LevelDBPlanBuilder::buildUnionAll(const TableInfo* pTableInfo, const ParseNode* pPredicate) {

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
			ScanPlanInfo info(pPredicate, pTableInfo);
			return std::move(info.m_pPlan);
		}
		rangePtrList[i] = rangeScanList.data() + i;
	}

	std::sort(rangePtrList.begin(), rangePtrList.end(),
			[](ScanPlanInfo* pInfo1, ScanPlanInfo* pInfo2) {
				return pInfo1->m_pScan->compareStart(pInfo2->m_pScan) < 0;
	});

	for (size_t i = 1; i < rangePtrList.size(); ++i) {
		auto pInfo = rangePtrList[i];
		auto pLastInfo = rangePtrList[i - 1];

		if(pInfo->m_pScan->startAfterEnd(pLastInfo->m_pScan)) {
			continue;
		}

		//key scan ranges have overlap, back to full scan
		ScanPlanInfo info(pPredicate, pTableInfo);
		return std::move(info.m_pPlan);
	}
	UnionAllPlan* pUnion = new UnionAllPlan(true);
	ExecutionPlanPtr pPlan(pUnion);

	auto pDBIter = LevelDBHandler::getHandler(pTableInfo)->createIterator();

	for(auto pScanInfo:rangePtrList) {
		pScanInfo->m_pScan->setLevelDBIterator(pDBIter);
		pUnion->addChildPlan(pScanInfo->m_pPlan.release());
	}

	return std::move(pPlan);
}

ExecutionPlanPtr LevelDBPlanBuilder::buildDeletePlan(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 2);

	const ParseNode* pTable = pNode->getChild(0);
	assert(pTable);

	const ParseNode* pPredicate = pNode->getChild(1);
	if(pPredicate != nullptr) {
		if(pPredicate->isFalseConst()) {
			return ExecutionPlanPtr(new EmptyPlan());
		} else if(pPredicate->isTrueConst()) {
			pPredicate = nullptr;
		}
	}

	assert( pTable->m_type == NodeType::NAME );

	auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->getString());
	if (pTableInfo == nullptr) {
		PARSE_ERROR("table ", pTable->getString(), " not found");
	}
	if(pTableInfo->getKeyCount() == 0) {
		PARSE_ERROR("DELETE is not supported for table ", pTable->getString());
	}

	auto pPlan = buildSelectPlan(pTableInfo, pPredicate);

	return ExecutionPlanPtr(new LevelDBDeletePlan(pTableInfo, pPlan.release()));
}

ExecutionPlanPtr LevelDBPlanBuilder::buildInsertPlan(const ParseNode* pNode)
{
	assert(pNode && pNode->children() >= 3);
	const ParseNode* pTable = pNode->getChild(0);
	const ParseNode* pColumn = pNode->getChild(1);
	const ParseNode* pValue = pNode->getChild(2);

	if(pColumn != nullptr) {
		PARSE_ERROR("insert partial columns is not supported");
	}
	assert(pTable && pTable->m_type == NodeType::NAME);

	const TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->getString());
	assert(pTableInfo != nullptr);

	auto pValuePlan = buildPlan(pValue);

	return ExecutionPlanPtr(new LevelDBInsertPlan(pTableInfo, pValuePlan.release()));
}


ExecutionPlanPtr LevelDBPlanBuilder::buildSelectPlan(const TableInfo* pTableInfo, const ParseNode* pPredicate) {
	assert(pTableInfo->getKeyCount() > 0);
	if (pPredicate == nullptr) {
		return ExecutionPlanPtr(new LevelDBScanPlan(pTableInfo));
	} else if (pPredicate->getOp() == Operation::OR) {
		return buildUnionAll(pTableInfo, pPredicate);
	} else {
		ScanPlanInfo info(pPredicate, pTableInfo);
		return std::move(info.m_pPlan);
	}
}

