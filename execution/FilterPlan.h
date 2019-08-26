#pragma once

#include "execution/BasePlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"
#include <vector>
#include <sstream>


class FilterPlan: public SingleChildPlan {
public:
	FilterPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Limit, pPlan) {}


	virtual void explain(std::vector<std::string>& rows)override {
		SingleChildPlan::explain(rows);
		std::string s = "Filter ";
		for (size_t i = 0; i < m_predicate.size(); ++i) {
			s.append(m_predicate[i].m_sExpr);
			s.append(", ");
		}
		rows.push_back(s);
	}

	virtual void begin()override {
		SingleChildPlan::begin();
		m_iCurrent = 0;
	}
	virtual bool next() override;


	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_pPlan->getResultColumns();
	}

	virtual std::string_view getProjectionName(size_t index)override {
		return m_pPlan->getProjectionName(index);
	}

	virtual DBDataType getResultType(size_t index)override {
		return m_pPlan->getResultType(index);
	}

	virtual std::string getInfoString() override {
		return ConcateToString("SELECT ", m_iCurrent);
	}

	virtual void getResult(size_t index, ExecutionResult& result) override {
		m_pPlan->getResult(index, result);
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns) override {
		m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(const ParseNode* pNode) override {
		return m_pPlan->addProjection(pNode);
	}

	struct PredicateInfo {
		std::string m_sColumn;
		std::string m_sExpr;
		int m_iOpCode;
		size_t m_iSubIndex;
		const ParseNode* m_pValue;
	};

	void addPredicate(const ParseNode* pPredicate);

private:
	uint64_t m_iCurrent = 0;
	std::vector<PredicateInfo> m_predicate;
};
