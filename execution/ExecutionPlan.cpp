#include "ExecutionPlan.h"
#include "execution/ExecutionException.h"
#include "common/ParseException.h"


ExecutionPlan::ExecutionPlan(PlanType type) :	m_type(type) {
}
ExecutionPlan::~ExecutionPlan() = default;



