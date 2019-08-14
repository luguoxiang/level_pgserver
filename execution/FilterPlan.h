#pragma once

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/Log.h"
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
		std::ostringstream os;
		os << "SELECT " << m_iCurrent;
		return os.str();
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

	void addPredicate(ParseNode* pPredicate) {
		if (pPredicate->m_type != NodeType::OP || pPredicate->children() != 2) {
			PARSE_ERROR("Unsupported predicate '%s'", pPredicate->m_sExpr.c_str());
		}
		assert(pPredicate);
		assert(pPredicate->children() == 2);
		assert(pPredicate->m_type == NodeType::OP);
		PredicateInfo info;
		info.m_sColumn = pPredicate->m_children[0]->m_sExpr;
		info.m_sExpr = pPredicate->m_sExpr;
		int i = m_pPlan->addProjection(pPredicate->m_children[0]);
		if (i < 0) {
			PARSE_ERROR("Unrecognized column '%s'", info.m_sColumn.c_str());
		}
		info.m_iSubIndex = i;
		info.m_iOpCode = OP_CODE(pPredicate);
		info.m_pValue = pPredicate->m_children[1];
		m_predicate.push_back(info);
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) override {
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, pOrder);
	}
private:
	std::unique_ptr<ExecutionPlan> m_pPlan;
	uint64_t m_iCurrent;
	std::vector<PredicateInfo> m_predicate;
};
