/*
 * GroupByPlan.cpp
 *
 *  Created on: 2012-5-13
 *      Author: guoxiang
 */

#include "GroupByPlan.h"

GroupByPlan::GroupByPlan(ExecutionPlan* pPlan)
		: ExecutionPlan(GroupBy), m_pPlan(pPlan), m_iRows(0)
{
	assert(m_pPlan);
	m_typeMap["sum"] = SUM;
	m_typeMap["avg"] = AVG;
	m_typeMap["count"] = COUNT;
	m_typeMap["max"] = MAX;
	m_typeMap["min"] = MIN;
}

GroupByPlan::~GroupByPlan()
{
	delete m_pPlan;
}

void GroupByPlan::explain(std::vector<std::string>& rows)
{
	m_pPlan->explain(rows);
	std::string s = "GroupBy(";
	for (size_t i = 0; i < m_groupby.size(); ++i)
	{
		s += m_pPlan->getProjectionName(m_groupby[i]);
		if (i + 1 < m_groupby.size())
		{
			s += ",";
		}
	}
	s += ") project:";
	for (size_t i = 0; i < m_proj.size(); ++i)
	{
		s += m_proj[i].m_pszName;
		if (i + 1 < m_proj.size())
		{
			s += ",";
		}
	}


	rows.push_back(s);
}

void GroupByPlan::begin()
{
	m_iRows = 0;
	m_last.clear();
	m_type.clear();
	m_pPlan->begin();
	m_bHasMore = m_pPlan->next();
	if (m_bHasMore)
	{
		for (size_t i = 0; i < m_groupby.size(); ++i)
		{
			ResultInfo result;
			m_pPlan->getResult(m_groupby[i], &result);
			DBDataType type = m_pPlan->getResultType(m_groupby[i]);
			m_last.push_back(result);
			m_type.push_back(type);
		}
	}
}

bool GroupByPlan::next()
{
	if (!m_bHasMore)
		return false;

	bool sameGroup = true;

	for (size_t i = 0; i < m_proj.size(); ++i)
	{
	    GroupByPlan::AggrFunc& proj = m_proj[i];

		m_pPlan->getResult(proj.m_iIndex, &proj.m_value);
		proj.m_iCount = 1;
	}
	while (sameGroup)
	{
		m_bHasMore = m_pPlan->next();
		if (!m_bHasMore)
			break;
		for (size_t i = 0; i < m_last.size(); ++i)
		{
			ResultInfo result;
			m_pPlan->getResult(m_groupby[i], &result);
			if (result.compare(m_last[i], m_type[i]) != 0)
			{
				m_last[i] = result;
				sameGroup = false;
			}
		}
		if (sameGroup)
		{
			for (size_t i = 0; i < m_proj.size(); ++i)
			{
				GroupByPlan::AggrFunc& proj = m_proj[i];
				proj.m_iCount++;

				if (proj.m_func == FIRST || proj.m_func == COUNT)
				{
					continue;
				}
				DBDataType type = m_pPlan->getResultType(proj.m_iIndex);
				ResultInfo info;
				m_pPlan->getResult(proj.m_iIndex, &info);
				if (proj.m_func == MIN || proj.m_func == MAX)
				{
					int n = info.compare(proj.m_value, type);
					if (n < 0 && proj.m_func == MIN)
					{
						proj.m_value = info;
					}
					else if (n > 0 && proj.m_func == MAX)
					{
						proj.m_value = info;
					}
				}
				else
				{
					if (!proj.m_value.add(info, type))
					{
						PARSE_ERROR("sum and avg is not supported on current parameter data type");
					}
				}
			}
		}
	}
	++m_iRows;
	return true;
}

void GroupByPlan::end()
{
	m_pPlan->end();
}

int GroupByPlan::addProjection(ParseNode* pNode)
{
	if (pNode->m_iType == NAME_NODE)
	{
		int i = m_pPlan->addProjection(pNode);
		if (i < 0)
		{
			PARSE_ERROR("Unrecognized projection column '%s'",
					pNode->m_pszValue);
		}
		AggrFunc func;
		func.m_func = FIRST;
		func.m_pszName = pNode->m_pszValue;
		func.m_iIndex = i;
		m_proj.push_back(func);
		return m_proj.size() - 1;
	}
	else if (pNode->m_iType == FUNC_NODE)
	{
		std::map<std::string, FuncType>::iterator iter = m_typeMap.find(
				pNode->m_pszValue);
		if (iter == m_typeMap.end())
		{
			PARSE_ERROR("Unknown function %s", pNode->m_pszValue);
		}
		AggrFunc func;
		func.m_func = iter->second;
		assert(pNode->m_iChildNum == 1);
		int i = m_pPlan->addProjection(pNode->m_children[0]);
		if (i < 0)
		{
			PARSE_ERROR("Unrecognized projection column '%s'",
					pNode->m_children[0]->m_pszExpr);
		}
		func.m_pszName = pNode->m_pszExpr;
		func.m_iIndex = i;
		m_proj.push_back(func);
		return m_proj.size() - 1;
	}
	else
	{
		return -1;
	}
}

