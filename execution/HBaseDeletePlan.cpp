#include "HBaseDeletePlan.h"
#include <stdio.h>
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionException.h"
#include "execution/ObConnection.h"
#include "WorkThreadInfo.h"
#include "execution/ParseTools.h"

HBaseDeletePlan::HBaseDeletePlan(TableInfo* pTable, ExecutionPlan* pSearchPlan) 
: ExecutionPlan(HBaseDelete), m_pTableInfo(pTable)
, m_pSearchPlan(pSearchPlan), m_iDeleteRows(0)
{
	assert(pSearchPlan);

	m_pInfo = (WorkThreadInfo*) pthread_getspecific(WorkThreadInfo::tls_key);
	assert(m_pInfo);
}

HBaseDeletePlan::~HBaseDeletePlan()
{
	if(m_pSearchPlan)
	{
		delete m_pSearchPlan;
		m_pSearchPlan = 0;
	}
}

void HBaseDeletePlan::explain(std::vector<std::string>& rows)
{
	m_pSearchPlan->explain(rows);
	std::string s = "hbase:delete table=";
	s.append(m_pTableInfo->getName());
	rows.push_back(s);
}

void HBaseDeletePlan::cancel()
{
	m_pSearchPlan->cancel();
}

void HBaseDeletePlan::begin()
{
	m_pSearchPlan->begin();
	m_iDeleteRows = 0;

}

bool HBaseDeletePlan::next()
{
	if(!m_pSearchPlan->next()) return false;
	ResultInfo info;
	m_pSearchPlan->getResult(m_pTableInfo->getColumnCount(), &info);

	try
	{
		HBaseConnection& conn = m_pInfo->getHBaseConnection();
		HbaseClient* pClient = conn.getClient();
		std::string sTableName(m_pTableInfo->getName());
		std::string row(info.m_value.m_pszResult, info.m_len);
		pClient->deleteAllRow(sTableName,row);
	}
	catch(const IOError& e)
	{
			throw new ExecutionException(e.message.c_str(), false);
	}
	catch(const TException& ioe)
	{
			throw new ExecutionException(ioe.what(), false);
	}

	++m_iDeleteRows;
	if((m_iDeleteRows % 10000) == 0)
	{
		LOG(INFO, "ob_delete: table=%s,count=%d",
				m_pTableInfo->getName(), m_iDeleteRows);
	}
	return true;
}

void HBaseDeletePlan::end()
{
	m_pSearchPlan->end();

#ifndef NO_TIMEING
	++m_pInfo->m_iSqlCount;
#endif
}
