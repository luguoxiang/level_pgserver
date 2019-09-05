#include "ExecutionPlan.h"
#include <cassert>
#include <sstream>
#include "execution/ExecutionException.h"
#include "execution/WorkThreadInfo.h"
#include "common/ParseException.h"


ExecutionPlan::ExecutionPlan(PlanType type) :
		m_type(type) {
}
ExecutionPlan::~ExecutionPlan() = default;


void ExecutionPlan::checkCancellation() {
	if(WorkThreadInfo::isCanceled()) {
		EXECUTION_ERROR("canceled");
	}
}
