#pragma once

#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"

class ConstPlan: public ExecutionPlan
{
public:
	ConstPlan() : ExecutionPlan(Const), m_iCurrent(0)
	{
	}

	virtual void explain(std::vector<std::string>& rows);

	virtual void begin()
	{
		assert(!m_rows.empty());
		m_iCurrent = 0;
	}

	virtual bool next()
	{
		return m_iCurrent++ < m_rows.size();
	}

	virtual void end()
	{
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns()
	{
		return m_columns.size();
	}

	virtual const char* getProjectionName(size_t index)
	{
		return m_columns[index].c_str();
	}

	virtual DBDataType getResultType(size_t index);

	virtual void getInfoString(char* szBuf, int len)
	{
		snprintf(szBuf, len, "SELECT %lu", m_rows.size());
	}

	virtual void getResult(size_t index, ResultInfo* pInfo);

	virtual void getAllColumns(std::vector<const char*>& columns)
	{
		for(size_t i=0;i<m_columns.size();++i)
		{
			columns.push_back(m_columns[i].c_str());
		}
	}

	virtual int addProjection(ParseNode* pNode);

	void addRow(ParseNode* pRow);

private:
	uint64_t m_iCurrent;
	std::vector<std::string> m_columns;
	std::vector<ParseNode*> m_rows;
};
