#include "common/MetaConfig.h"
#include "common/ConfigInfo.h"
#include "common/ParseException.h"
#include "execution/BuildPlan.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ExplainPlan.h"

ExecutionPlanPtr buildPlanForLevelDBInsert(const ParseNode* pNode);

ExecutionPlanPtr buildPlanForConst(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForUnionAll(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForDesc(const ParseNode* pNode);

ExecutionPlanPtr buildPlan(const ParseNode* pNode) {
	if(pNode->m_type != NodeType::PLAN) {
		PARSE_ERROR("WRONG NODE ", pNode->m_sExpr);
	}
	switch(pNode->m_op){
	case Operation::SHOW_TABLES:
		return ExecutionPlanPtr(new ShowTables());
	case Operation::DESC_TABLE:
		return buildPlanForDesc(pNode);
	case Operation::WORKLOAD:
		return ExecutionPlanPtr(new WorkloadResult());

	case Operation::SELECT: {
		const ParseNode* pTable = pNode->getChild(SQL_SELECT_TABLE);
		assert(pTable);
		auto pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
		if (pTableInfo == nullptr) {
			PARSE_ERROR("table ", pTable->m_sValue, " not found");
		}
		if(pTableInfo->getKeyCount() > 0) {
			LevelDBSelectPlanBuilder builder(pTableInfo);
			return builder.build(pNode);
		} else {
			SelectPlanBuilder builder(pTableInfo);
			return builder.build(pNode);
		}
	}
	case Operation::INSERT:
		return buildPlanForLevelDBInsert(pNode);
	case Operation::EXPLAIN:{
		assert(pNode->children() == 1);
		auto pPlan = buildPlan(pNode->getChild(0));
		return ExecutionPlanPtr(new ExplainPlan(pPlan.release()));
	}
	case Operation::UNION_ALL:
		return buildPlanForUnionAll(pNode);
	case Operation::VALUES:
		assert(pNode->children() == 1);
		return buildPlanForConst(pNode->getChild(0));

	case Operation::SELECT_WITH_SUBQUERY:{
		const ParseNode* pQuery = pNode->getChild(SQL_SELECT_TABLE);
		auto pSource = buildPlan(pQuery);
		SelectPlanBuilder builder(pSource.release());
		return builder.build(pNode);
	}
	default:
		PARSE_ERROR("not supported: ", pNode->m_sExpr);
		return nullptr;
	}

}
