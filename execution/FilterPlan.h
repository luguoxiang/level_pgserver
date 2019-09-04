#pragma once

#include "execution/BasePlan.h"
#include "common/ParseException.h"
#include <vector>
#include <sstream>
#include <memory>
#include <set>

class FilterPlan: public SingleChildPlan {
public:
	FilterPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Limit, pPlan) {}


	virtual void explain(std::vector<std::string>& rows)override {
		SingleChildPlan::explain(rows);
		rows.push_back("Filter OR");

		assert(!m_predicatesInOr.empty());
		for (auto& pAnd: m_predicatesInOr) {
			std::string s = "  ";
			assert(!pAnd->empty());
			for(auto& info : *pAnd) {
				s.append(info.m_sExpr);
				s.append(" and ");
			}
			s.erase (s.end()- 5, s.end());
			rows.push_back(s);
		}
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
		std::string m_sExpr;
		Operation m_op;
		int m_iLeftIndex;
		int m_iRightIndex;
		ExecutionResult m_leftConst;
		ExecutionResult m_rightConst;
	};

	bool addPredicate(const ParseNode* pNode, std::set<std::string_view>* pIgnore = nullptr);
	void clearPredicates() {
		m_predicatesInOr.clear();
	}
	void setPredicate(const ParseNode* pNode);
private:
	bool evaluate(const PredicateInfo& info);
	using AndPredicateListPtr = std::unique_ptr<std::vector<PredicateInfo>>;

	void doAddPredicate(std::vector<PredicateInfo>& andList, const ParseNode* pNode, std::set<std::string_view>* pIgnore);


	uint64_t m_iCurrent = 0;

	std::vector<AndPredicateListPtr> m_predicatesInOr;
};
