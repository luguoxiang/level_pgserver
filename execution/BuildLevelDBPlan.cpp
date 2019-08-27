#include "common/BuildPlan.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/LevelDBInsertPlan.h"

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

	LevelDBInsertPlan* pPlan = new LevelDBInsertPlan(pTableInfo, Tools::popPlan());
	Tools::pushPlan(pPlan);
}
