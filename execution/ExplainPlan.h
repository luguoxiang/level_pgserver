#pragma once

#include "ParseTools.h"
#include "ExecutionPlan.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>

class ExplainPlan: public ExecutionPlan {
public:
	ExplainPlan(ExecutionPlan* pPlan) : ExecutionPlan(PlanType::Explain), m_iCurrentRow(0), m_pPlan(pPlan) {
		assert(pPlan);
	}



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

	virtual void getResult(size_t columnIndex, ExecutionResult* pInfo)override;

private:
	std::vector<std::string> m_rows;
	size_t m_iCurrentRow;
	std::unique_ptr<ExecutionPlan> m_pPlan;
};

