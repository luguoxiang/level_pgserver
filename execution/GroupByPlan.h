
#pragma once

#include "ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"
#include "common/Log.h"

class GroupByPlan: public ExecutionPlan
{
public:
	GroupByPlan(ExecutionPlan* pPlan);
	virtual ~GroupByPlan();

	enum FuncType
	{
		FIRST, SUM, AVG, COUNT, MAX, MIN
	};
	struct AggrFunc
	{
		FuncType m_func;
		const char* m_pszName;
		size_t m_iIndex;
		size_t m_iCount;
		ResultInfo m_value;
	};

	virtual void explain(std::vector<std::string>& rows);

	virtual void getInfoString(char* szBuf, int len)
	{
		snprintf(szBuf, len, "SELECT %lu", m_iRows);
	}

	virtual int getResultColumns()
	{
		return m_proj.size();
	}

	virtual DBDataType getResultType(size_t index)
	{
		AggrFunc func = m_proj[index];
		switch (func.m_func)
		{
		case FIRST:
		case MIN:
		case MAX:
		case SUM:
		case AVG:
			return m_pPlan->getResultType(func.m_iIndex);
		case COUNT:
			return TYPE_INT64;
		default:
			assert(0);
			return TYPE_UNKNOWN;
		}
	}

	virtual void getResult(size_t index, ResultInfo* pInfo)
	{
		AggrFunc func = m_proj[index];
		switch (func.m_func)
		{
		case FIRST:
		case MIN:
		case MAX:
		case SUM:
			*pInfo = func.m_value;
			break;
		case COUNT:
			pInfo->m_bNull = false;
			pInfo->m_value.m_lResult = func.m_iCount;
			break;
		case AVG:
			*pInfo = func.m_value;
			pInfo->div(m_proj[index].m_iCount,
					m_pPlan->getResultType(func.m_iIndex));
			break;
		default:
			assert(0);
			break;
		}
	}

	virtual int addProjection(ParseNode* pNode);

	virtual void getAllColumns(std::vector<const char*>& columns)
	{
		for (size_t i = 0; i < m_proj.size(); ++i)
		{
			columns.push_back(m_proj[i].m_pszName);
		}
	}

	virtual const char* getProjectionName(size_t index)
	{
		return m_proj[index].m_pszName;
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn,
			bool* pOrder)
	{
		return m_pPlan->ensureSortOrder(iSortIndex, pszColumn, pOrder);
	}

	virtual void begin();
	virtual bool next();
	virtual void end();

	void addGroupByColumn(ParseNode* pNode)
	{
		int i = m_pPlan->addProjection(pNode);
		if (i < 0)
		{
			PARSE_ERROR("Unrecognized sort column '%s'", pNode->m_pszExpr);
		}
		m_groupby.push_back(i);
	}

private:
	ExecutionPlan* m_pPlan;
	std::vector<AggrFunc> m_proj;
	std::vector<size_t> m_groupby;
	size_t m_iRows;
	bool m_bHasMore;

	std::vector<ResultInfo> m_last;
	std::vector<DBDataType> m_type;
	std::map<std::string, FuncType> m_typeMap;
};

