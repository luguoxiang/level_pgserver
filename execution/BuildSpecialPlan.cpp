#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ShowColumns.h"

void buildPlanForDesc(const ParseNode* pNode) {
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
	Tools::pushPlan(new ShowColumns(pEntry));
}

void buildPlanForWorkload(const ParseNode* pNode) {
	Tools::pushPlan(new WorkloadResult());
}

void buildPlanForShowTables(const ParseNode* pNode) {
	Tools::pushPlan(new ShowTables());
}
