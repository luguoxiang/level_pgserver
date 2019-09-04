#include "ExecutionPlan.h"
#include <cassert>
#include <sstream>
#include "execution/ExecutionException.h"
#include "common/ParseException.h"


ExecutionPlan::ExecutionPlan(PlanType type) :
		m_type(type) {
}
ExecutionPlan::~ExecutionPlan() = default;


