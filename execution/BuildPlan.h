#pragma once

#include "common/ParseNode.h"
#include "execution/ExecutionPlan.h"

constexpr int SQL_SELECT_PROJECT = 0;
constexpr int SQL_SELECT_TABLE = 1;
constexpr int SQL_SELECT_PREDICATE = 2;
constexpr int SQL_SELECT_GROUPBY = 3;
constexpr int SQL_SELECT_HAVING = 4;
constexpr int SQL_SELECT_ORDERBY = 5;
constexpr int SQL_SELECT_LIMIT = 6;

class LevelDBPlanBuilder {
public:
	ExecutionPlanPtr buildSelectPlan(const ParseNode* pNode);
	ExecutionPlanPtr buildDeletePlan(const ParseNode* pNode);
protected:
	void buildPlanForOrderBy(const ParseNode* pNode);
	void buildPlanForProjection(const ParseNode* pNode);
	void buildPlanForGroupBy(const ParseNode* pNode);
	void buildPlanForLimit(const ParseNode* pNode);
	void buildPlanForFilter(const ParseNode* pNode);
	void buildPlanForReadFile(const TableInfo* pTableInfo);
	const ParseNode* buildPlanForLevelDB(const TableInfo* pTableInfo, const ParseNode* pPredicate);

	void buildFullScan(const ParseNode* pNode);
	const ParseNode* buildUnionAll(const TableInfo* pTableInfo, const ParseNode* pPredicate);

	ExecutionPlanPtr m_pPlan;
};



ExecutionPlanPtr buildPlan(const ParseNode* pNode);
