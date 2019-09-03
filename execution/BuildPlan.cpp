#include "common/MetaConfig.h"
#include "common/ConfigInfo.h"
#include "common/ParseException.h"
#include "execution/BuildPlan.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ExplainPlan.h"

ExecutionPlanPtr buildPlanForLevelDBInsert(const ParseNode* pNode);

ExecutionPlanPtr buildPlanForConst(const ParseNode* pNode);
ExecutionPlanPtr buildPlanForDesc(const ParseNode* pNode);

ExecutionPlanPtr buildPlan(const ParseNode* pNode) {
	if(pNode->m_type != NodeType::PLAN) {
		PARSE_ERROR("WRONG NODE ", pNode->m_sExpr);
	}
	switch(pNode->getOp()){
	case Operation::SHOW_TABLES:
		return ExecutionPlanPtr(new ShowTables());
	case Operation::DESC_TABLE:
		return buildPlanForDesc(pNode);
	case Operation::WORKLOAD:
		return ExecutionPlanPtr(new WorkloadResult());

	case Operation::SELECT: {
		SelectPlanBuilder builder;
		return builder.build(pNode);
	}
	case Operation::INSERT:
		return buildPlanForLevelDBInsert(pNode);
	case Operation::EXPLAIN:{
		assert(pNode->children() == 1);
		auto pPlan = buildPlan(pNode->getChild(0));
		return ExecutionPlanPtr(new ExplainPlan(pPlan.release()));
	}
	case Operation::VALUES:
		assert(pNode->children() == 1);
		return buildPlanForConst(pNode->getChild(0));
	default:
		PARSE_ERROR("not supported: ", pNode->m_sExpr);
		return nullptr;
	}

}
