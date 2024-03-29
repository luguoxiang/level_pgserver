#pragma once

#include "BasePlan.h"
#include "ExecutionResult.h"
#include "common/ParseException.h"
#include <vector>
#include <sstream>
#include <memory>
#include <set>
#include <absl/strings/str_cat.h>

class FilterPlan: public SingleChildPlan {
public:
	FilterPlan(ExecutionPlan* pPlan) : SingleChildPlan(PlanType::Filter, pPlan) {}


	virtual void explain(std::vector<std::string>& rows, size_t depth) override;

	virtual void begin(const std::atomic_bool& bTerminated)override {
		SingleChildPlan::begin(bTerminated);
		m_iCurrent = 0;
	}
	
	virtual bool next(const std::atomic_bool& bTerminated) override;

	virtual std::string getInfoString() override {
		return absl::StrCat("SELECT ", m_iCurrent);
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
