#pragma once

#include "ExecutionPlan.h"
#include "common/ParseException.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <absl/strings/str_cat.h>

class UnionAllPlan: public ExecutionPlan {
public:
	UnionAllPlan(bool hasOrder) :
			ExecutionPlan(PlanType::Other), m_hasOrder(hasOrder) {
	}
	virtual ~UnionAllPlan() {
		for(auto pPlan : m_plans) {
			delete pPlan;
		}
	}
	void addChildPlan(ExecutionPlan* pPlan) {
		assert(pPlan != nullptr);
		m_plans.push_back(pPlan);
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override {
		rows.push_back(absl::StrCat(std::string(depth, '\t'), "UnionAll"));
		for (auto& pPlan : m_plans) {
			pPlan->explain(rows, 1 + depth);
		}
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int addProjection(const ParseNode* pColumn) override {
		assert(!m_plans.empty());
		int lastProj = -1;
		for (auto& pPlan : m_plans) {
			int proj = pPlan->addProjection(pColumn);
			if(proj <0) {
				return -1;
			}
			if(lastProj >= 0 && proj != lastProj) {
				PARSE_ERROR("projection conflict for ", pColumn->m_sExpr);
			}
			lastProj = proj;
		}
		return lastProj;
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
		return absl::StrCat("SELECT ", m_iCurrentRow);
	}

	virtual void getResult(size_t columnIndex, ExecutionResult& result)
			override;

	virtual bool ensureSortOrder(size_t iSortIndex,
			const std::string_view& sColumn, SortOrder order) override {
		if(!m_hasOrder) {
			return false;
		}

		for (auto& pPlan : m_plans) {
			if(!pPlan->ensureSortOrder(iSortIndex, sColumn, order)) {
				return false;
			}
		}

		m_order = order;
		return true;
	}
private:
	std::vector<ExecutionPlan*> m_plans;
	uint64_t m_iCurrentRow = 0;
	size_t m_iCurrentIndex = 0;
	bool m_hasOrder;
	SortOrder m_order = SortOrder::Any;
};

