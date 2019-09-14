#pragma once

#include <stdio.h>
#include <vector>
#include <absl/strings/str_cat.h>
#include "execution/BasePlan.h"


class WorkloadResult: public LeafPlan {
public:
	WorkloadResult() :
		LeafPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override{
		rows.push_back(absl::StrCat(std::string(depth, '\t'), "Workload"));
	}

	virtual std::string getInfoString() override;

	virtual std::string_view getProjectionName(size_t index) override;
	virtual DBDataType getResultType(size_t index) override;

	virtual void begin(const std::atomic_bool& bTerminated)override;

	virtual bool next(const std::atomic_bool& bTerminated)override;

	virtual int getResultColumns()override;

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type)override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override;

	virtual int addProjection(const ParseNode* pNode)override;
private:
	size_t m_iIndex = 0;
	std::string m_tid;
};

