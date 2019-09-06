#pragma once

#include "execution/ExecutionPlan.h"

class ParseNode;

class LevelDBPlanBuilder {
public:
	ExecutionPlanPtr buildSelectPlan(const TableInfo* pTableInfo, const ParseNode* pPredicate);
	ExecutionPlanPtr buildDeletePlan(const ParseNode* pNode);
	ExecutionPlanPtr buildInsertPlan(const ParseNode* pNode);

private:
	ExecutionPlanPtr buildUnionAll(const TableInfo* pTableInfo, const ParseNode* pPredicate);


};
