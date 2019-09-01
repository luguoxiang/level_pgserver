
#include "BasePlan.h"
#include "ExecutionException.h"


LeafPlan::LeafPlan(PlanType type) : ExecutionPlan(type){

}

bool LeafPlan::next() {
	if (m_bCancel) {
		EXECUTION_ERROR("canceled");
	}
	return false;
}
