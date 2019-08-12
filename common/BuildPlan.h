#pragma once

#include "ParseNode.h"

extern void buildPlanDefault(ParseNode* pNode);

extern void buildPlanForFileSelect(ParseNode* pNode);

extern void buildPlanForLeftJoin(ParseNode* pNode);
extern void buildPlanForExplain(ParseNode* pNode);
extern void buildPlanForGroupBy(ParseNode* pNode);
extern void buildPlanForOrderBy(ParseNode* pNode);
extern void buildPlanForLimit(ParseNode* pNode);
extern void buildPlanForFilter(ParseNode* pNode);
extern void buildPlanForProjection(ParseNode* pNode);
extern void buildPlanForConst(ParseNode* pNode);
extern void buildPlanForUnionAll(ParseNode* pNode);
extern void buildPlanForDesc(ParseNode* pNode);
extern void buildPlanForWorkload(ParseNode* pNode);
extern void buildPlanForShowTables(ParseNode* pNode);
extern void buildPlanForReadFile(ParseNode* pNode);

