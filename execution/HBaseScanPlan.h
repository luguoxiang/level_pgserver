#ifndef HBASE_SCAN_PLAN_H
#define HBASE_SCAN_PLAN_H

#include "common/ConfigInfo.h"
#include "execution/ParseTools.h"
#include "execution/ExecutionPlan.h"
#include "execution/RowKeyRange.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdio.h>

class WorkThreadInfo;
class HBaseScanPlan: public ExecutionPlan
{
public:
	HBaseScanPlan(TableInfo* pInfo);
	virtual ~HBaseScanPlan();

	virtual int addProjection(ParseNode* pColumn);

	virtual void getAllColumns(std::vector<const char*>& columns);

	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual void cancel();

	virtual void explain(std::vector<std::string>& rows);

	virtual const char* getProjectionName(size_t index)
	{
		if(index < m_pTableInfo->getColumnCount()) return m_pTableInfo->getColumn(index)->getName();
		return "rowkey";
	}

	virtual DBDataType getResultType(size_t index)
	{
		if(index < m_pTableInfo->getColumnCount()) return m_pTableInfo->getColumn(index)->m_type;
		if(index == m_pTableInfo->getColumnCount()) return TYPE_BYTES;
		if(index == m_pTableInfo->getColumnCount() + 1) return TYPE_DATETIME;
		return TYPE_UNKNOWN;
	}

	virtual int getResultColumns()
	{
		return m_pTableInfo->getColumnCount() + 1;//with rowkey
	}

	virtual void getInfoString(char* szBuf, int len)
	{
		snprintf(szBuf, len, "SELECT %lu", m_lRows);
	}

	virtual void getResult(size_t columnIndex, ResultInfo* pInfo);

	virtual bool ensureSortOrder(size_t iSortIndex, const char* pszColumn, bool* pOrder);

	void setRowKeyRange(RowKeyRange* pRowKeyRange)
	{
		m_pRowKeyRange.reset(pRowKeyRange);
	}

	TableInfo* getTableInfo()
	{
		return m_pTableInfo;
	}

private:
	std::auto_ptr<RowKeyRange> m_pRowKeyRange;
	
	int64_t m_lRows;

	TableInfo* m_pTableInfo;

	WorkThreadInfo* m_pInfo;
	bool		m_bCancel;
	int			m_iScanner;

	std::vector<TRowResult> m_value;
	TRowResult	m_current;
	int			m_iIndex;
	std::string m_cf;
	std::vector<std::string> m_valueColumns;
};

#endif //HBASE_SCAN_PLAN_H
