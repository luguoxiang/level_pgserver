#pragma once

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "ExecutionPlan.h"
#include "WorkThreadInfo.h"

class WorkloadResult: public ExecutionPlan
{
public:
	WorkloadResult()
			: ExecutionPlan(Other)
	{
	}

	virtual void explain(std::vector<std::string>& rows)
	{
		rows.push_back("Workload");
	}

	virtual void getInfoString(char* szBuf, int len);

	virtual const char* getProjectionName(size_t index);
	virtual DBDataType getResultType(size_t index);

	virtual void begin();

	virtual bool next();

	virtual void end()
	{
	}

	virtual int getResultColumns()
	{
		return 7;
	}

	virtual void getResult(size_t index, ResultInfo* pInfo);

	virtual void getAllColumns(std::vector<const char*>& columns);

	virtual int addProjection(ParseNode* pNode);
private:
	size_t m_iIndex;
	std::string m_tid;
};

