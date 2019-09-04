#pragma once

#include "common/Exception.h"
#include "execution/BasePlan.h"
#include <vector>
#include <sstream>

class LimitPlan: public SingleChildPlan {
public:
	LimitPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Limit, pPlan) {}


	virtual void explain(std::vector<std::string>& rows, size_t depth) override {
		std::ostringstream os;
		os << std::string(depth, '\t');
		os << "Limit "<< m_iLimit<< ","<< m_iOffset;
		rows.push_back(os.str());

		SingleChildPlan::explain(rows, depth);
	}

	virtual void begin() override{
		SingleChildPlan::begin();
		m_iCurrent = 0;
	}
	virtual bool next() override;

	virtual std::string getInfoString() override{
		return ConcateToString("SELECT ", m_iCurrent - m_iOffset);
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
