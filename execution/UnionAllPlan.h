#pragma once

#include "ParseTools.h"
#include "ExecutionPlan.h"
#include "ScanColumn.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdio.h>
#include <stdarg.h>

class UnionAllPlan: public ExecutionPlan {
public:
	UnionAllPlan(ExecutionPlan* pLeft, ExecutionPlan* pRight) :
			ExecutionPlan(PlanType::Explain), m_pLeft(pLeft), m_pRight(pRight) {
		assert(m_pLeft && m_pRight);
	}


	virtual void explain(std::vector<std::string>& rows) {
		m_pLeft->explain(rows);
		m_pRight->explain(rows);
		rows.push_back("Union All");
	}

	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual int addProjection(ParseNode* pColumn) {
		return m_pLeft->addProjection(pColumn);
	}

	virtual const char* getProjectionName(size_t index) {
		return m_pLeft->getProjectionName(index);
	}

	virtual void getAllColumns(std::vector<const char*>& columns) {
		return m_pLeft->getAllColumns(columns);
	}

	virtual DBDataType getResultType(size_t index) {
		DBDataType type = m_pLeft->getResultType(index);
		switch (type) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			return DBDataType::INT64;
		default:
			return type;
		}
	}

	virtual int getResultColumns() {
		return m_pLeft->getResultColumns();
	}

	virtual void getInfoString(char* szBuf, int len);

	virtual void getResult(size_t columnIndex, ResultInfo* pInfo);

private:
	std::unique_ptr<ExecutionPlan> m_pLeft;
	std::unique_ptr<ExecutionPlan> m_pRight;
	uint64_t m_iCurrentRow = 0;
	bool m_bLeftDone = false;
};

