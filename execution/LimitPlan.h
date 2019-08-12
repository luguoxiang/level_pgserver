#pragma once

#include "execution/ExecutionPlan.h"
#include <vector>
#include <stdio.h>

class LimitPlan: public ExecutionPlan {
public:
	LimitPlan(ExecutionPlan* pPlan) : ExecutionPlan(PlanType::Limit), m_pPlan(pPlan), m_iLimit(0), m_iOffset(0), m_iCurrent(
					0) {
		assert(pPlan);
	}


	virtual void explain(std::vector<std::string>& rows) {
		m_pPlan->explain(rows);
		char buf[100];
		snprintf(buf, 100, "Limit %llu, Offset %llu", m_iLimit, m_iOffset);
		rows.push_back(buf);
	}

	virtual void begin() {
		m_pPlan->begin();
		m_iCurrent = 0;
	}
	virtual bool next();

	virtual void end() {
		return m_pPlan->end();
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns() {
		return m_pPlan->getResultColumns();
	}

	virtual const char* getProjectionName(size_t index) {
		return m_pPlan->getProjectionName(index);
	}

	virtual DBDataType getResultType(size_t index) {
		return m_pPlan->getResultType(index);
	}

	virtual void getInfoString(char* szBuf, int len) {
		snprintf(szBuf, len, "SELECT %llu", m_iCurrent - m_iOffset);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo) {
		m_pPlan->getResult(index, pInfo);
	}

	virtual void getAllColumns(std::vector<const char*>& columns) {
		m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(ParseNode* pNode) {
		return m_pPlan->addProjection(pNode);
	}

	void setLimit(uint64_t iLimit, uint64_t iOffset) {
		m_iLimit = iLimit;
		m_iOffset = iOffset;
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn,
			bool* pOrder) {
		return m_pPlan->ensureSortOrder(iSortIndex, pszColumn, pOrder);
	}
private:
	std::unique_ptr<ExecutionPlan> m_pPlan;
	uint64_t m_iLimit;
	uint64_t m_iOffset;
	uint64_t m_iCurrent;
};
