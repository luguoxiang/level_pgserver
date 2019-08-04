#ifndef PROJECTION_PLAN_H
#define PROJECTION_PLAN_H

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include <vector>
#include <map>
class ProjectionPlan: public ExecutionPlan
{
	struct ProjectionInfo
	{
		size_t m_iSubIndex;
		const char* m_pszName;
		const char* m_pszRaw;
	};
	typedef std::map<const char*, size_t, Tools::StringCompare> IndexMap;
public:
	ProjectionPlan(ExecutionPlan* pPlan)
			: ExecutionPlan(Projection), m_pPlan(pPlan)
	{
		assert(pPlan);
	}

	virtual ~ProjectionPlan()
	{
		delete m_pPlan;
	}

	virtual void explain(std::vector<std::string>& rows)
	{
		m_pPlan->explain(rows);
		std::string s = "Projection ";
		for (size_t i = 0; i < m_proj.size(); ++i)
		{
			s.append(m_proj[i].m_pszName);
			s.append(", ");
		}
		rows.push_back(s);
	}

	bool project(ParseNode* pNode, const char* pszName);

	virtual void getAllColumns(std::vector<const char*>& columns)
	{
		for (size_t i = 0; i < m_proj.size(); ++i)
		{
			columns.push_back(m_proj[i].m_pszName);
		}
	}

	virtual int addProjection(ParseNode* pNode)
	{
		IndexMap::iterator iter = m_map.find(pNode->m_pszExpr);
		if (iter == m_map.end())
			return -1;
		return iter->second;
	}

	virtual void begin()
	{
		m_pPlan->begin();
	}
	virtual bool next()
	{
		return m_pPlan->next();
	}

	virtual void end()
	{
		return m_pPlan->end();
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn,
			bool* pOrder)
	{
		IndexMap::iterator iter = m_map.find(pszColumn);
		if (iter == m_map.end())
			return false;
		size_t iIndex = iter->second;
		return m_pPlan->ensureSortOrder(iSortIndex, m_proj[iIndex].m_pszRaw, pOrder);
	}
	/*
	 * number of projection column
	 */
	virtual int getResultColumns()
	{
		return m_proj.size();
	}

	virtual const char* getProjectionName(size_t index)
	{
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_proj[index].m_pszName;
	}

	virtual DBDataType getResultType(size_t index)
	{
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResultType(iSubIndex);
	}

	virtual void getInfoString(char* szBuf, int len)
	{
		return m_pPlan->getInfoString(szBuf, len);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo)
	{
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResult(iSubIndex, pInfo);
	}

	void setChild(ExecutionPlan* pPlan)
	{
		m_pPlan = pPlan;
	}
private:
	std::vector<ProjectionInfo> m_proj;
	std::map<const char*, size_t, Tools::StringCompare> m_map;
	ExecutionPlan* m_pPlan;
};
#endif  // PROJECTION_PLAN_H
