#include "common/MetaConfig.h"
#include "common/ConfigInfo.h"
#include "common/ParseException.h"
#include "execution/BuildPlan.h"


ExecutionPlanPtr buildPlanForLevelDBInsert(const ParseNode* pNode);

ExecutionPlanPtr buildPlanForExplain(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForConst(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForUnionAll(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForDesc(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForWorkload(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForShowTables(const ParseNode* pNode);

ExecutionPlanPtr buildPlan(const ParseNode* pNode) {
	if(pNode->m_type != NodeType::PLAN) {
		PARSE_ERROR("WRONG NODE ", pNode->m_sExpr);
	}
	switch(pNode->m_op){
	case Operation::SHOW_TABLES:
		return buildPlanForShowTables(pNode);
	case Operation::DESC_TABLE:
		return buildPlanForDesc(pNode);
	case Operation::WORKLOAD:
		return buildPlanForWorkload(pNode);

	case Operation::SELECT: {
		const ParseNode* pTable = pNode->getChild(SQL_SELECT_TABLE);
		auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
		if (pTableInfo == nullptr) {
			PARSE_ERROR("table ", pTable->m_sValue, " not found");
		}

		SelectPlanBuilder builder(pTableInfo);
		return builder.build(pNode);
	}
	case Operation::INSERT:
		return buildPlanForLevelDBInsert(pNode);
	case Operation::EXPLAIN:
		return buildPlanForExplain(pNode);
	case Operation::UNION_ALL:
		return buildPlanForUnionAll(pNode);
	case Operation::VALUES:
		assert(pNode->children() == 1);
		return buildPlanForConst(pNode->getChild(0));

	case Operation::SELECT_WITH_SUBQUERY:{
		const ParseNode* pQuery = pNode->getChild(SQL_SELECT_TABLE);
		auto pSource = buildPlan(pQuery);
		SelectPlanBuilder builder(pSource);
		return builder.build(pNode);
	}
	default:
		PARSE_ERROR("not supported: ", pNode->m_sExpr);
		return nullptr;
	}

}
