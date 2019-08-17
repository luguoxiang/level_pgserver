#pragma once

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"
#include <vector>
#include <sstream>


class FilterPlan: public ExecutionPlan {
public:
	FilterPlan(ExecutionPlan* pPlan) : ExecutionPlan(PlanType::Limit), m_pPlan(pPlan), m_iCurrent(0) {
		assert(pPlan);
	}


	virtual void explain(std::vector<std::string>& rows)override {
		m_pPlan->explain(rows);
		std::string s = "Filter ";
		for (size_t i = 0; i < m_predicate.size(); ++i) {
			s.append(m_predicate[i].m_sExpr);
			s.append(", ");
		}
		rows.push_back(s);
	}

	virtual void begin()override {
		m_pPlan->begin();
		m_iCurrent = 0;
	}
	virtual bool next() override;

	virtual void end()override {
		return m_pPlan->end();
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_pPlan->getResultColumns();
	}

	virtual std::string getProjectionName(size_t index)override {
		return m_pPlan->getProjectionName(index);
	}

	virtual DBDataType getResultType(size_t index)override {
		return m_pPlan->getResultType(index);
	}

	virtual std::string getInfoString() override {
		return ConcateToString("SELECT ", m_iCurrent);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo) override {
		m_pPlan->getResult(index, pInfo);
	}

	virtual void getAllColumns(std::vector<std::string>& columns) override {
		m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(ParseNode* pNode) override {
		return m_pPlan->addProjection(pNode);
	}

	struct PredicateInfo {
		std::string m_sColumn;
		std::string m_sExpr;
		int m_iOpCode;
		size_t m_iSubIndex;
		ParseNode* m_pValue;
	};

	void addPredicate(ParseNode* pPredicate);

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) override {
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, pOrder);
	}
private:
	std::unique_ptr<ExecutionPlan> m_pPlan;
	uint64_t m_iCurrent;
	std::vector<PredicateInfo> m_predicate;
};
