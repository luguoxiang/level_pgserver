#include "common/BuildPlan.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/LevelDBInsertPlan.h"
#include "execution/ConstPlan.h"

void buildPlanForConst(const ParseNode* pNode) {
	ConstPlan* pConst = new ConstPlan();
	Tools::pushPlan(pConst);
	const ParseNode* pLastRow = nullptr;
	for (size_t i=0;i<pNode->children(); ++i) {
		auto pRow = pNode->getChild(i);
		if (pLastRow != nullptr && pLastRow->children() != pRow->children()) {
			PARSE_ERROR("Values column number does not match: expect ", pLastRow->children(), " but got ", pRow->children());
		}
		pConst->addRow(pRow);
		pLastRow = pRow;
	}
}


void buildPlanForLevelDBInsert(const ParseNode* pNode)
{
	assert(pNode && pNode->children() >= 3);
	const ParseNode* pTable = pNode->getChild(0);
	const ParseNode* pColumn = pNode->getChild(1);
	const ParseNode* pValue = pNode->getChild(2);

	if(pColumn != nullptr) {
		EXECUTION_ERROR("insert partial columns is not supported");
	}
	assert(pTable && pTable->m_type == NodeType::NAME);

	const TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	assert(pTableInfo != nullptr);

	BUILD_PLAN(pValue);

	auto pValuePlan = Tools::popPlan();
	ExecutionPlanPtr pPlan(new LevelDBInsertPlan(pTableInfo, pValuePlan));
	Tools::pushPlan(pPlan);
}
