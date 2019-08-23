#pragma once

#include "ParseTools.h"
#include "ExecutionPlan.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>

class UnionAllPlan: public ExecutionPlan {
public:
	UnionAllPlan(ExecutionPlan* pLeft, ExecutionPlan* pRight) :
			ExecutionPlan(PlanType::Other), m_pLeft(pLeft), m_pRight(pRight) {
		assert(m_pLeft && m_pRight);
	}


	virtual void explain(std::vector<std::string>& rows) override{
		m_pLeft->explain(rows);
		m_pRight->explain(rows);
		rows.push_back("Union All");
	}

	virtual void begin()override;
	virtual bool next()override;
	virtual void end()override;

	virtual int addProjection(const ParseNode* pColumn)override {
		return m_pLeft->addProjection(pColumn);
	}

	virtual std::string_view getProjectionName(size_t index)override {
		return m_pLeft->getProjectionName(index);
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		return m_pLeft->getAllColumns(columns);
	}

	virtual DBDataType getResultType(size_t index)override {
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

	virtual int getResultColumns() override{
		return m_pLeft->getResultColumns();
	}

	virtual std::string getInfoString()override {
		return ConcateToString("SELECT ", m_iCurrentRow);
	}

	virtual void getResult(size_t columnIndex, ExecutionResult* pInfo) override;

	virtual void cancel()override {
		m_pLeft->cancel();
		m_pRight->cancel();
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			bool* pOrder) override {
		return false;
	}
private:
	std::unique_ptr<ExecutionPlan> m_pLeft;
	std::unique_ptr<ExecutionPlan> m_pRight;
	uint64_t m_iCurrentRow = 0;
	bool m_bLeftDone = false;
};

