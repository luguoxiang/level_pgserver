#pragma once

#include "execution/BasePlan.h"
#include <vector>
#include <absl/strings/str_cat.h>
#include <absl/strings/substitute.h>

class LimitPlan: public SingleChildPlan {
public:
	LimitPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Limit, pPlan) {}


	virtual void explain(std::vector<std::string>& rows, size_t depth) override {
		rows.push_back(absl::Substitute("$0Limit $1, $2", std::string(depth, '\t'),m_iLimit,m_iOffset));

		SingleChildPlan::explain(rows, depth);
	}

	virtual void begin() override{
		SingleChildPlan::begin();
		m_iCurrent = 0;
	}
	virtual bool next() override;

	virtual std::string getInfoString() override{
		return absl::StrCat("SELECT ", m_iCurrent - m_iOffset);
	}

	void setLimit(uint64_t iLimit, uint64_t iOffset) {
		m_iLimit = iLimit;
		m_iOffset = iOffset;
	}

private:
	uint64_t m_iLimit = 0;
	uint64_t m_iOffset = 0;
	uint64_t m_iCurrent = 0;
};
