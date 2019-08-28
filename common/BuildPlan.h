#pragma once

#include "ParseNode.h"

extern void buildPlanDefault(const ParseNode* pNode);

extern void buildPlanForFileSelect(const ParseNode* pNode);
extern void buildPlanForLevelDBInsert(const ParseNode* pNode);

extern void buildPlanForExplain(const ParseNode* pNode);
extern void buildPlanForGroupBy(const ParseNode* pNode);
extern void buildPlanForOrderBy(const ParseNode* pNode);
extern void buildPlanForLimit(const ParseNode* pNode);
extern void buildPlanForFilter(const ParseNode* pNode);
extern void buildPlanForProjection(const ParseNode* pNode);
extern void buildPlanForConst(const ParseNode* pNode);
extern void buildPlanForUnionAll(const ParseNode* pNode);
extern void buildPlanForDesc(const ParseNode* pNode);
extern void buildPlanForWorkload(const ParseNode* pNode);
extern void buildPlanForShowTables(const ParseNode* pNode);

