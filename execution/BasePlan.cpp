
#include "BasePlan.h"
#include "ExecutionException.h"

SingleChildPlan::SingleChildPlan(PlanType type, ExecutionPlanPtr& pChild) : ExecutionPlan(type), m_pPlan(pChild) {
	assert(pChild);
}

LeafPlan::LeafPlan(PlanType type) : ExecutionPlan(type){

}

bool LeafPlan::next() {
	if (m_bCancel) {
		EXECUTION_ERROR("canceled");
	}
	return false;
}
