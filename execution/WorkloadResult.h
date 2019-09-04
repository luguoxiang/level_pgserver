#pragma once

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "BasePlan.h"
#include "WorkThreadInfo.h"

class WorkloadResult: public LeafPlan {
public:
	WorkloadResult() :
		LeafPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override{
		std::ostringstream os;
		os << std::string(depth, '\t');
		os << "Workload";
		rows.push_back(os.str());
	}

	virtual std::string getInfoString() override;

	virtual std::string_view getProjectionName(size_t index) override;
	virtual DBDataType getResultType(size_t index) override;

	virtual void begin()override;

	virtual bool next()override;

	virtual int getResultColumns()override {
		return 7;
	}

	virtual void getResult(size_t index, ExecutionResult& result)override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override;

	virtual int addProjection(const ParseNode* pNode)override;
private:
	size_t m_iIndex = 0;
	std::string m_tid;
};

