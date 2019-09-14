#pragma once

#include "execution/BasePlan.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <absl/strings/str_cat.h>

class ExplainPlan: public SingleChildPlan {
public:
	ExplainPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Other, pPlan), m_iCurrentRow(0) {	}

	virtual void explain(std::vector<std::string>& rows, size_t depth)override {
	}

	virtual void begin(const std::atomic_bool& bTerminated)override ;
	virtual bool next(const std::atomic_bool& bTerminated)override;
	virtual void end()override;

	virtual std::string_view getProjectionName(size_t index) override{
		return "explain";
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		columns.push_back("explain");
	}

	virtual DBDataType getResultType(size_t index) override{
		return DBDataType::STRING;
	}

	virtual int getResultColumns() override{
		return 1;
	}

	virtual std::string getInfoString()override {
		return absl::StrCat("EXPLAIN ", m_rows.size());
	}

	virtual void getResult(size_t columnIndex, ExecutionResult& result, DBDataType type)override;

private:
	std::vector<std::string> m_rows;
	size_t m_iCurrentRow;
};

