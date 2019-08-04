#ifndef INSERTPLAN_H
#define INSERTPLAN_H
#include <stdio.h>
#include "execution/ImplicitRowKeyRange.h"
#include "ExecutionPlan.h"
#include <vector>

class WorkThreadInfo;
class ImplicitRowKeyRange;
class RowKeyRange;

class HBaseInsertPlan: public ExecutionPlan
{
public:
	HBaseInsertPlan(TableInfo* pTableInfo);
	virtual ~HBaseInsertPlan();

	void setValuePlan(ExecutionPlan* pDataPlan);

	virtual void explain(std::vector<std::string>& rows);

	void addColumn(DBColumnInfo* pColumn);

	size_t getInsertColumnCount()
	{
		return m_columns.size();
	}

	virtual void begin();
	virtual bool next();
	virtual void end();
	virtual void cancel();

	virtual void getInfoString(char* szBuf, int len)
	{
		snprintf(szBuf, len, "INSERT 0 %lu", m_iInsertRows);
	}

	virtual int getResultColumns()
	{
		return 0;
	}

private:
	void doInsert(ImplicitRowKeyRange& rowkey, const char* pszTable);
	void doExplain(ImplicitRowKeyRange& rowkey, const char* pszTable, std::vector<std::string>& rows);

	void checkValueForInteger(DBColumnInfo* pColumn, ResultInfo& info, DBDataType type);
	void checkValueForString(DBColumnInfo* pColumn, ResultInfo& info, DBDataType type);

	void buildRowKey(ImplicitRowKeyRange& rowkey);
	void prepareParseNode();
	void commitData();

	TableInfo* m_pTableInfo;

	std::vector<DBColumnInfo*> m_columns;
	ExecutionPlan* m_pDataPlan;
	ParseNode* m_pRow;

	std::vector<BatchMutation> m_mutations;

	size_t m_iInsertRows;

	WorkThreadInfo* m_pInfo;
	std::string m_cf;
};

#endif //INSERTPLAN_H
