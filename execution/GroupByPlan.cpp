/*
 * GroupByPlan.cpp
 *
 *  Created on: 2012-5-13
 *      Author: guoxiang
 */

#include "GroupByPlan.h"

GroupByPlan::GroupByPlan(ExecutionPlan* pPlan) :
		ExecutionPlan(PlanType::GroupBy), m_pPlan(pPlan) {
	assert(m_pPlan);
	m_typeMap["sum"] = FuncType::SUM;
	m_typeMap["avg"] = FuncType::AVG;
	m_typeMap["count"] = FuncType::COUNT;
	m_typeMap["max"] = FuncType::MAX;
	m_typeMap["min"] = FuncType::MIN;
}


void GroupByPlan::explain(std::vector<std::string>& rows) {
	m_pPlan->explain(rows);
	std::string s = "GroupBy(";
	for (size_t i = 0; i < m_groupby.size(); ++i) {
		s += m_pPlan->getProjectionName(m_groupby[i]);
		if (i + 1 < m_groupby.size()) {
			s += ",";
		}
	}
	s += ") project:";
	for (size_t i = 0; i < m_proj.size(); ++i) {
		s += m_proj[i].m_sName;
		if (i + 1 < m_proj.size()) {
			s += ",";
		}
	}

	rows.push_back(s);
}

void GroupByPlan::begin() {
	m_iRows = 0;
	m_last.clear();
	m_type.clear();
	m_pPlan->begin();
	m_bHasMore = m_pPlan->next();
	if (m_bHasMore) {
		for (size_t i = 0; i < m_groupby.size(); ++i) {
			ExecutionResult result;
			m_pPlan->getResult(m_groupby[i], &result);
			//make sure result will be valid after next() call
			result.cache();

			DBDataType type = m_pPlan->getResultType(m_groupby[i]);
			m_last.push_back(result);
			m_type.push_back(type);
		}
	}
}

bool GroupByPlan::next() {
	if (!m_bHasMore)
		return false;

	bool sameGroup = true;

	for (size_t i = 0; i < m_proj.size(); ++i) {
		GroupByPlan::AggrFunc& proj = m_proj[i];

		m_pPlan->getResult(proj.m_iIndex, &proj.m_value);
		proj.m_iCount = 1;
	}
	while (sameGroup) {
		m_bHasMore = m_pPlan->next();
		if (!m_bHasMore)
			break;
		for (size_t i = 0; i < m_last.size(); ++i) {
			ExecutionResult result;
			m_pPlan->getResult(m_groupby[i], &result);
			//make sure result will be valid after next() call
			result.cache();

			if (result.compare(m_last[i], m_type[i]) != 0) {
				m_last[i] = result;
				sameGroup = false;
			}
		}
		if (sameGroup) {
			for (size_t i = 0; i < m_proj.size(); ++i) {
				GroupByPlan::AggrFunc& proj = m_proj[i];
				proj.m_iCount++;

				if (proj.m_func == FuncType::FIRST || proj.m_func == FuncType::COUNT) {
					continue;
				}
				DBDataType type = m_pPlan->getResultType(proj.m_iIndex);
				ExecutionResult info;
				m_pPlan->getResult(proj.m_iIndex, &info);
				//type should not be string or bytes, no need to cache result

				if (proj.m_func == FuncType::MIN || proj.m_func == FuncType::MAX) {
					int n = info.compare(proj.m_value, type);
					if (n < 0 && proj.m_func == FuncType::MIN) {
						proj.m_value = info;
					} else if (n > 0 && proj.m_func == FuncType::MAX) {
						proj.m_value = info;
					}
				} else {
					if (!proj.m_value.add(info, type)) {
						throw new ParseException(
								"sum and avg is not supported on current parameter data type");
					}
				}
			}
		}
	}
	++m_iRows;
	return true;
}

void GroupByPlan::end() {
	m_pPlan->end();
}

int GroupByPlan::addProjection(ParseNode* pNode) {
	if (pNode->m_type == NodeType::NAME) {
		if(int i = m_pPlan->addProjection(pNode); i >= 0 ) {
			AggrFunc func;
			func.m_func = FuncType::FIRST;
			func.m_sName = pNode->m_sValue;
			func.m_iIndex = i;
			m_proj.push_back(func);
			return m_proj.size() - 1;
		} else {
			throw new ParseException(ConcateToString("Unrecognized projection column ", pNode->m_sExpr));
		}

	} else if (pNode->m_type == NodeType::FUNC) {
		if(auto iter = m_typeMap.find(pNode->m_sValue); iter != m_typeMap.end()) {
			AggrFunc func;
			func.m_func = iter->second;
			assert(pNode->children() == 1);
			if (int i = m_pPlan->addProjection(pNode->m_children[0]); i>=0) {
				func.m_sName = pNode->m_sExpr;
				func.m_iIndex = i;
				m_proj.push_back(func);
				return m_proj.size() - 1;
			} else{
				throw new ParseException(ConcateToString("Unrecognized projection column ", pNode->m_children[0]->m_sExpr));
			}
		} else {
			throw new ParseException(ConcateToString("Unknown function ", pNode->m_sExpr));
		}

	} else {
		return -1;
	}
}

