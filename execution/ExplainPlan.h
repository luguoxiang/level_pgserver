#pragma once

#include "ParseTools.h"
#include "ExecutionPlan.h"
#include "ScanColumn.h"
#include "RowKeyRange.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdio.h>
#include <stdarg.h>

class ExplainPlan: public ExecutionPlan
{
public:
	ExplainPlan(ExecutionPlan* pPlan)
			: ExecutionPlan(Explain), m_iCurrentRow(0), m_pPlan(pPlan)
	{
		assert(pPlan);
	}

	virtual ~ExplainPlan()
	{
		delete m_pPlan;
	}

	virtual void explain(std::vector<std::string>& rows)
	{
	}

	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual const char* getProjectionName(size_t index)
	{
		return "explain";
	}

	virtual void getAllColumns(std::vector<const char*>& columns)
	{
		columns.push_back("explain");
	}

	virtual DBDataType getResultType(size_t index)
	{
		return TYPE_STRING;
	}

	virtual int getResultColumns()
	{
		return 1;
	}

	virtual void getInfoString(char* szBuf, int len);

	virtual void getResult(size_t columnIndex, ResultInfo* pInfo);

private:
	std::vector<std::string> m_rows;
	size_t m_iCurrentRow;
	ExecutionPlan* m_pPlan;
};


