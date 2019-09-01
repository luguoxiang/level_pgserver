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

class SelectPlanBuilder {
public:
	SelectPlanBuilder(const TableInfo* pTableInfo);
	SelectPlanBuilder(ExecutionPlan* pSubQuery) : m_pPlan(pSubQuery) {};
	SelectPlanBuilder() {}
	ExecutionPlanPtr build(const ParseNode* pNode);

protected:
	void buildPlanForOrderBy(const ParseNode* pNode);
	void buildPlanForProjection(const ParseNode* pNode);
	void buildPlanForGroupBy(const ParseNode* pNode);
	void buildPlanForLimit(const ParseNode* pNode);
	void buildPlanForFilter(const ParseNode* pNode);
	ExecutionPlanPtr m_pPlan;
};

class LevelDBSelectPlanBuilder : public SelectPlanBuilder {
public:
	LevelDBSelectPlanBuilder(const TableInfo* pTableInfo) : m_pTableInfo(pTableInfo) {}
	ExecutionPlanPtr build(const ParseNode* pNode);
private:
	void buildFullScan(const ParseNode* pNode);
	void buildUnionAll(const ParseNode* pNode);
	const TableInfo* m_pTableInfo;
};

ExecutionPlanPtr buildPlan(const ParseNode* pNode);
