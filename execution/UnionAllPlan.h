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
	UnionAllPlan() :
			ExecutionPlan(PlanType::Other) {
	}

	void addChildPlan(ExecutionPlanPtr& pPlan) {
		assert(pPlan != nullptr);
		m_plans.push_back(pPlan);
	}

	virtual void explain(std::vector<std::string>& rows) override {
		for (auto& pPlan : m_plans) {
			pPlan->explain(rows);
		}
		rows.push_back("Union All");
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int addProjection(const ParseNode* pColumn) override {
		assert(!m_plans.empty());
		return m_plans[0]->addProjection(pColumn);
	}

	virtual std::string_view getProjectionName(size_t index) override {
		assert(!m_plans.empty());
		return m_plans[0]->getProjectionName(index);
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns)
			override {
		assert(!m_plans.empty());
		return m_plans[0]->getAllColumns(columns);
	}

	virtual DBDataType getResultType(size_t index) override {
		assert(!m_plans.empty());
		DBDataType type = m_plans[0]->getResultType(index);
		switch (type) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			return DBDataType::INT64;
		default:
			return type;
		}
	}

	virtual int getResultColumns() override {
		assert(!m_plans.empty());
		return m_plans[0]->getResultColumns();
	}

	virtual std::string getInfoString() override {
		return ConcateToString("SELECT ", m_iCurrentRow);
	}

	virtual void getResult(size_t columnIndex, ExecutionResult& result)
			override;

	virtual void cancel() override {
		for (auto& pPlan : m_plans) {
			pPlan->cancel();
		}
	}

	virtual bool ensureSortOrder(size_t iSortIndex,
			const std::string_view& sColumn, SortOrder order) override {
		return false;
	}
private:
	std::vector<ExecutionPlanPtr> m_plans;
	uint64_t m_iCurrentRow = 0;
	size_t m_iCurrentIndex = 0;
};

