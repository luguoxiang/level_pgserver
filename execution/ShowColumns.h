#pragma once

#include <stdio.h>
#include <vector>
#include <absl/strings/str_cat.h>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"

class ShowColumns: public LeafPlan {
public:
	ShowColumns(const TableInfo* pEntry) :
		LeafPlan(PlanType::Other), m_pEntry(pEntry) {
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override {
		rows.push_back(absl::StrCat(std::string(depth, '\t'), "ShowColumns"));
	}

	virtual void begin(const std::atomic_bool& bTerminated) override;

	virtual bool next(const std::atomic_bool& bTerminated) override;

	virtual void end() override{
	}

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type)override;

	virtual std::string_view getProjectionName(size_t index)override;

	virtual DBDataType getResultType(size_t index)override;

	virtual int getResultColumns() override;

	virtual std::string getInfoString()override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override;

	virtual int addProjection(const ParseNode* pNode)override;
private:
	size_t m_iIndex = 0;
	const TableInfo* m_pEntry;
};
