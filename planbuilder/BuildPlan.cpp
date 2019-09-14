#include "BuildPlan.h"
#include "LevelDBPlanBuilder.h"
#include "SelectPlanBuilder.h"

#include "common/MetaConfig.h"
#include "common/ConfigInfo.h"
#include "common/ParseException.h"
#include "execution/ShowTables.h"
#include "execution/ExplainPlan.h"
#include "execution/ConstPlan.h"
#include "execution/ShowColumns.h"


namespace {

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

		pEntry = MetaConfig::getInstance().getTableInfo(pTable->getString());
	} else {
		pEntry = MetaConfig::getInstance().getTableInfo(pTable->getString());
	}
	if (pEntry == nullptr) {
		PARSE_ERROR("Undefined table ", pTable->getString());
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

}

ExecutionPlanPtr buildPlan(const ParseNode* pNode) {
	assert(pNode->m_type == NodeType::PLAN);
	switch(pNode->getOp()){
	case Operation::SHOW_TABLES:
		return ExecutionPlanPtr(new ShowTables());
	case Operation::DESC_TABLE:
		return buildPlanForDesc(pNode);
	case Operation::SELECT: {
		SelectPlanBuilder builder;
		return builder.build(pNode);
	}
	case Operation::DELETE: {
		LevelDBPlanBuilder builder;
		return builder.buildDeletePlan(pNode);
	}
	case Operation::INSERT: {
		LevelDBPlanBuilder builder;
		return builder.buildInsertPlan(pNode);
	}
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
