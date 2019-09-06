#pragma once

#include "common/ParseNode.h"
#include "execution/ExecutionPlan.h"

ExecutionPlanPtr buildPlan(const ParseNode* pNode);
