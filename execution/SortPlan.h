#pragma once

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"
#include "common/Log.h"

class SortPlan: public ExecutionPlan
{
	struct SortProjection
	{
		size_t m_iSubIndex;
		const char* m_pszName;
	};
public:
	SortPlan(ExecutionPlan* pPlan);

	virtual ~SortPlan()
	{
		delete m_pPlan;
	}

	virtual void explain(std::vector<std::string>& rows)
	{
		m_pPlan->explain(rows);
		std::string s = "Sort(";
		for (size_t i = 0; i < m_sort.size(); ++i)
		{
			s.append(m_sort[i].m_pszColumn);
			s.append(" ");
			switch(m_sort[i].m_order)
			{
			case Ascend:
			case Any:
				s.append("ascend");
				break;
			case Descend:
				s.append("descend");
				break;
			default:
				assert(0);
				break;
			}
			if (i == m_sort.size() - 1)
				s.append(") ");
			else
				s.append(", ");
		}
		s += "project:";
		for (size_t i = 0; i < m_proj.size(); ++i)
		{
			s.append(m_proj[i].m_pszName);
			if (i < m_proj.size() - 1)
				s.append(", ");
		}

		rows.push_back(s);
	}

	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual int getResultColumns()
	{
		return m_proj.size();
	}

	virtual const char* getProjectionName(size_t index)
	{
		return m_proj[index].m_pszName;
	}

	virtual DBDataType getResultType(size_t index)
	{
		return m_pPlan->getResultType(m_proj[index].m_iSubIndex);
	}

	virtual void getInfoString(char* szBuf, int len)
	{
		return m_pPlan->getInfoString(szBuf, len);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo);

	virtual void getAllColumns(std::vector<const char*>& columns)
	{
		return m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(ParseNode* pNode)
	{
		int index = m_pPlan->addProjection(pNode);
		if (index < 0)
			return index;

		for (size_t i = 0; i < m_proj.size(); ++i)
		{
			if (m_proj[i].m_iSubIndex == index)
				return i;
		}
		SortProjection proj;
		proj.m_iSubIndex = index;
		proj.m_pszName = pNode->m_pszExpr;
		m_proj.push_back(proj);
		return m_proj.size() - 1;
	}

	/**
	 * SortPlan support an order named Any, normally it is same with Ascend,
	 * But it can be upgraded to Ascend or Descend. It is used in following situation:
	 * select a from (select * from t) group by a order by a desc;
	 * group by will generate a SortPlan so that it can do sort group-by.
	 * Because this sql has a 'order by a desc', it better to sort by desc order,
	 * But BuildGroupBy plan is called before BuildSortPlan, it has no idea about the latter sort 
	 * requirement. At this time, Any order take effect:
	 * GroupBy Plan can generate a SortPlan which sort a by order Any.
	 * later, when BuilSortPlan query SortPlan::ensureSortOrder,
	 * Any order can be upgrated to Descend order.
	 */
	enum SortOrder
	{
		Ascend, Descend, Any
	};

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn, bool* pOrder)
	{
		if(m_sort.size() <= iSortIndex) return false;

		SortSpec& spec = m_sort[iSortIndex];

		if(strcmp(pszColumn, spec.m_pszColumn) != 0) return false;

		if(pOrder == NULL) return true;
		switch(spec.m_order)
		{
		case Ascend:
			return *pOrder;
		case Descend:
			return *pOrder == false;
		case Any:
			spec.m_order = *pOrder ? Ascend : Descend;
			return true;
		};
	}

	void addSortSpecification(ParseNode* pNode, SortOrder order)
	{
		int i = addProjection(pNode);
		if (i < 0)
		{
			throw new ParseException("unrecognized column '%s'", pNode->m_pszExpr);
		}
		SortSpec spec;
		spec.m_iIndex = i;
		spec.m_pszColumn = pNode->m_pszExpr;
		spec.m_iSubIndex = m_proj[i].m_iSubIndex;
		spec.m_order = order;
		spec.m_type = m_pPlan->getResultType(spec.m_iSubIndex);
		m_sort.push_back(spec);
	}

private:
	struct SortSpec
	{
		size_t m_iIndex;
		size_t m_iSubIndex;
		const char* m_pszColumn;
		SortOrder m_order;
		DBDataType m_type;
	};
	struct Compare
	{
		Compare(size_t iColumns, const std::vector<SortSpec>& spec)
				: m_iColumns(iColumns), m_sort(spec)
		{
		}
		bool operator()(ResultInfo* pRow1, ResultInfo* pRow2);
	private:
		size_t m_iColumns;
		std::vector<SortSpec> m_sort;
	};

	ExecutionPlan* m_pPlan;
	std::vector<ResultInfo*> m_rows;
	std::vector<SortProjection> m_proj;
	std::vector<SortSpec> m_sort;
	int m_iCurrent;
};

