#ifndef DELETEPLAN_H
#define DELETEPLAN_H
#include "ExecutionPlan.h"
#include "RowKeyRange.h"
#include <stdio.h>
#include <memory>

class WorkThreadInfo;

class HBaseDeletePlan: public ExecutionPlan
{
public:
	HBaseDeletePlan(TableInfo* pTable, ExecutionPlan* pSearchPlan);
	~HBaseDeletePlan();

	virtual void explain(std::vector<std::string>& rows);

	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual void cancel();
	virtual int getResultColumns()
	{
		return 0;
	}

	virtual void getInfoString(char* szBuf, int len)
	{
		snprintf(szBuf, len, "DELETE %d", m_iDeleteRows);
	}


private:
	TableInfo* m_pTableInfo;

	ExecutionPlan* m_pSearchPlan;
	WorkThreadInfo* m_pInfo;

	size_t m_iDeleteRows;
};

#endif //DELETEPLAN_H
