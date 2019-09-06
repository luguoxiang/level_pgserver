#pragma once

#include "execution/ExecutionPlan.h"

class ParseNode;
class TableInfo;
class SelectPlanBuilder {
public:
	ExecutionPlanPtr build(const ParseNode* pNode);

private:
	ExecutionPlanPtr m_pPlan;
	void buildPlanForReadFile(const TableInfo* pTableInfo);
	void buildPlanForFilter(const ParseNode* pNode);
	void buildPlanForOrderBy(const ParseNode* pNode);
	void buildPlanForProjection(const ParseNode* pNode);
	void buildPlanForGroupBy(const ParseNode* pNode);
	void buildPlanForLimit(const ParseNode* pNode);
};
