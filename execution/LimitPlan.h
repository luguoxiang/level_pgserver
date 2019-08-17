#pragma once

#include "execution/ExecutionPlan.h"
#include <vector>
#include <sstream>

class LimitPlan: public ExecutionPlan {
public:
	LimitPlan(ExecutionPlan* pPlan) : ExecutionPlan(PlanType::Limit), m_pPlan(pPlan), m_iLimit(0), m_iOffset(0), m_iCurrent(
					0) {
		assert(pPlan);
	}


	virtual void explain(std::vector<std::string>& rows) override {
		m_pPlan->explain(rows);
		rows.push_back(ConcateToString("Limit ", m_iLimit, ",", m_iOffset));
	}

	virtual void begin() override{
		m_pPlan->begin();
		m_iCurrent = 0;
	}
	virtual bool next() override;

	virtual void end() override {
		return m_pPlan->end();
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_pPlan->getResultColumns();
	}

	virtual std::string getProjectionName(size_t index) override{
		return m_pPlan->getProjectionName(index);
	}

	virtual DBDataType getResultType(size_t index) override{
		return m_pPlan->getResultType(index);
	}

	virtual std::string getInfoString() override{
		return ConcateToString("SELECT ", m_iCurrent - m_iOffset);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo)override {
		m_pPlan->getResult(index, pInfo);
	}

	virtual void getAllColumns(std::vector<std::string>& columns) override{
		m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(ParseNode* pNode)override {
		return m_pPlan->addProjection(pNode);
	}

	void setLimit(uint64_t iLimit, uint64_t iOffset) {
		m_iLimit = iLimit;
		m_iOffset = iOffset;
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) override{
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, pOrder);
	}
private:
	std::unique_ptr<ExecutionPlan> m_pPlan;
	uint64_t m_iLimit;
	uint64_t m_iOffset;
	uint64_t m_iCurrent;
};
