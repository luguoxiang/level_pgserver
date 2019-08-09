#pragma once

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include <vector>
class FilterPlan: public ExecutionPlan {
public:
	FilterPlan(ExecutionPlan* pPlan) :
			ExecutionPlan(Limit), m_pPlan(pPlan), m_iCurrent(0) {
		assert(pPlan);
	}
	virtual ~FilterPlan() {
		delete m_pPlan;
	}

	virtual void explain(std::vector<std::string>& rows) {
		m_pPlan->explain(rows);
		std::string s = "Filter ";
		for (size_t i = 0; i < m_predicate.size(); ++i) {
			s.append(m_predicate[i].m_pszExpr);
			s.append(", ");
		}
		rows.push_back(s);
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
		snprintf(szBuf, len, "SELECT %llu", m_iCurrent);
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

	struct PredicateInfo {
		const char* m_pszColumn;
		const char* m_pszExpr;
		int m_iOpCode;
		size_t m_iSubIndex;
		ParseNode* m_pValue;
	};

	void addPredicate(ParseNode* pPredicate) {
		if (pPredicate->m_iType != OP_NODE || pPredicate->m_iChildNum != 2) {
			PARSE_ERROR("Unsupported predicate '%s'", pPredicate->m_pszExpr);
		}
		assert(pPredicate);
		assert(pPredicate->m_iChildNum == 2);
		assert(pPredicate->m_iType == OP_NODE);
		PredicateInfo info;
		info.m_pszColumn = pPredicate->m_children[0]->m_pszExpr;
		info.m_pszExpr = pPredicate->m_pszExpr;
		int i = m_pPlan->addProjection(pPredicate->m_children[0]);
		if (i < 0) {
			PARSE_ERROR("Unrecognized column '%s'", info.m_pszColumn);
		}
		info.m_iSubIndex = i;
		info.m_iOpCode = OP_CODE(pPredicate);
		info.m_pValue = pPredicate->m_children[1];
		m_predicate.push_back(info);
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn,
			bool* pOrder) {
		return m_pPlan->ensureSortOrder(iSortIndex, pszColumn, pOrder);
	}
private:
	ExecutionPlan* m_pPlan;
	uint64_t m_iCurrent;
	std::vector<PredicateInfo> m_predicate;
};
