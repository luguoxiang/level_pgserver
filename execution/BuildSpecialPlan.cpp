
#include "common/MetaConfig.h"
#include "common/ParseException.h"

#include "execution/ParseTools.h"
#include "execution/ShowColumns.h"
#include "execution/BuildPlan.h"

#include "execution/LevelDBInsertPlan.h"
#include "execution/ConstPlan.h"

ExecutionPlanPtr buildPlanForDesc(const ParseNode* pNode) {
	assert(pNode->children() == 1);
	auto pTable = pNode->getChild(0);

	const TableInfo* pEntry = nullptr;
	if (pTable->m_type != NodeType::NAME) {
		assert(pTable->children() == 2);

		auto pDB = pTable->getChild(0);
		pTable = pTable->getChild(1);

		assert(pDB->m_type == NodeType::NAME);
		assert(pTable->m_type == NodeType::NAME);

		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	} else {
		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	}
	if (pEntry == nullptr) {
		PARSE_ERROR("Undefined table ", pTable->m_sValue);
	}

	return ExecutionPlanPtr(new ShowColumns(pEntry));
}


ExecutionPlanPtr buildPlanForConst(const ParseNode* pNode) {
	ConstPlan* pConst = new ConstPlan();
	ExecutionPlanPtr pResult(pConst);
	const ParseNode* pLastRow = nullptr;
	for (size_t i=0;i<pNode->children(); ++i) {
		auto pRow = pNode->getChild(i);
		if (pLastRow != nullptr && pLastRow->children() != pRow->children()) {
			PARSE_ERROR("Values column number does not match: expect ", pLastRow->children(), " but got ", pRow->children());
		}
		pConst->addRow(pRow);
		pLastRow = pRow;
	}
	return pResult;
}


ExecutionPlanPtr buildPlanForLevelDBInsert(const ParseNode* pNode)
{
	assert(pNode && pNode->children() >= 3);
	const ParseNode* pTable = pNode->getChild(0);
	const ParseNode* pColumn = pNode->getChild(1);
	const ParseNode* pValue = pNode->getChild(2);

	if(pColumn != nullptr) {
		PARSE_ERROR("insert partial columns is not supported");
	}
	assert(pTable && pTable->m_type == NodeType::NAME);

	const TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	assert(pTableInfo != nullptr);

	auto pValuePlan = buildPlan(pValue);

	return ExecutionPlanPtr(new LevelDBInsertPlan(pTableInfo, pValuePlan.release()));
}

