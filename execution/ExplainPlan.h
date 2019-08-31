#pragma once

#include "execution/ParseTools.h"
#include "execution/BasePlan.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>

class ExplainPlan: public SingleChildPlan {
public:
	ExplainPlan(ExecutionPlanPtr& pPlan) : SingleChildPlan(PlanType::Other, pPlan), m_iCurrentRow(0) {	}

	virtual void explain(std::vector<std::string>& rows)override {
	}

	virtual void begin()override ;
	virtual bool next()override;
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
		return ConcateToString("EXPLAIN ", m_rows.size());
	}

	virtual void getResult(size_t columnIndex, ExecutionResult& result)override;

private:
	std::vector<std::string> m_rows;
	size_t m_iCurrentRow;
};

