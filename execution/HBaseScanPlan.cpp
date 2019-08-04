#include "HBaseScanPlan.h"
#include "ObConnection.h"
#include "HBaseConnection.h"
#include "common/ParseException.h"
#include "common/SqlParser.tab.h"
#include "common/ConfigException.h"
#include "common/Log.h"
#include "execution/ExecutionException.h"
#include "execution/ScanColumnImpl.h"
#include "WorkThreadInfo.h"
#include <algorithm>
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid) 

HBaseScanPlan::HBaseScanPlan(TableInfo* pInfo)
		: ExecutionPlan(HBaseScan), m_lRows(0)
		, m_pTableInfo(pInfo), m_bCancel(false)
{
	m_pInfo = (WorkThreadInfo*) pthread_getspecific(WorkThreadInfo::tls_key);
	assert(m_pInfo);
	if(m_pTableInfo->hasAttribute("cf"))
	{
		m_cf = m_pTableInfo->getAttribute("cf") + ":";
	}
	else
	{
		throw new ConfigException("hbase table missing cf attribute", false);
	}
}

HBaseScanPlan::~HBaseScanPlan()
{
}

void HBaseScanPlan::explain(std::vector<std::string>& rows)
{
	std::string s = "hbase:scan ";
	s += m_pTableInfo->getName();
	s += " ";

	if (m_pRowKeyRange.get())
	{
		const char* pszStart = Tools::byteToString(
				m_pRowKeyRange->getStartRowKey(),
				m_pRowKeyRange->getStartLength());
		const char* pszEnd = Tools::byteToString(m_pRowKeyRange->getEndRowKey(),
				m_pRowKeyRange->getEndLength());

		s.append("[");
		s.append(pszStart);
		s.append(",");
		s.append(pszEnd);
		s.append("]");
	}
	else
	{
		s.append("all");
	}

	rows.push_back(s);
	for (size_t i = 0; i < m_valueColumns.size(); ++i)
	{
		s = "hbase:scan_column ";
		s.append(m_valueColumns[i]);
		rows.push_back(s);
	}
}

void HBaseScanPlan::begin()
{
	m_lRows = 0;
	m_bCancel = false;
	m_value.clear();

	Log& log = Log::getLogger();


#ifndef NO_TIMEING
	struct timeval start, end;
	gettimeofday(&start, NULL);
#endif
	try
	{
	HBaseConnection& conn = m_pInfo->getHBaseConnection();
	HbaseClient* pClient = conn.getClient();
	std::string sTableName(m_pTableInfo->getName());

	if (m_pRowKeyRange.get() == NULL)
	{
		std::string startKey;
		std::string endKey;
		m_iScanner = pClient->scannerOpen(sTableName, "", m_valueColumns);
	}
	else
	{
		std::string startKey(m_pRowKeyRange->getStartRowKey(), m_pRowKeyRange->getStartLength());
		std::string endKey(m_pRowKeyRange->getEndRowKey(), m_pRowKeyRange->getEndLength());
		m_iScanner = pClient->scannerOpenWithStop(sTableName, startKey, endKey, m_valueColumns);
	}

	pClient->scannerGet(m_value, m_iScanner);
	}
	catch(const IOError& e)
	{
			throw new ExecutionException(e.message.c_str(), false);
	}
	catch(const TException& ioe)
	{
			throw new ExecutionException(ioe.what(), false);
	}

	m_iIndex = 0;

#ifndef NO_TIMEING
	gettimeofday(&end, NULL);

	uint64_t iTimeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec
			- start.tv_usec;
	if (iTimeuse > m_pInfo->m_iBiggestExec)
	{
		m_pInfo->m_iBiggestExec = iTimeuse;
	}

	m_pInfo->m_iExecScanTime += iTimeuse;
	++m_pInfo->m_iSqlCount;
#endif

}

bool HBaseScanPlan::next()
{
	if(m_bCancel || m_value.size() == 0) return false;
	m_current = m_value[m_iIndex];

	++m_iIndex;
	++m_lRows;

	if(m_iIndex == m_value.size())
	{
		HBaseConnection& conn = m_pInfo->getHBaseConnection();
		HbaseClient* pClient = conn.getClient();
		m_value.clear();
		pClient->scannerGet(m_value, m_iScanner);
		if(m_value.size() > 0)
		{
			m_iIndex = 0;
		}
	}
	return true;
}

void HBaseScanPlan::end()
{
	m_pRowKeyRange.reset(0);

}

void HBaseScanPlan::cancel()
{
	m_bCancel = true;
}

void HBaseScanPlan::getResult(size_t index, ResultInfo* pInfo)
{
	assert(pInfo);
	pInfo->m_bNull = false;
	if(index == m_pTableInfo->getColumnCount()) 
	{
			//ROWKEY
			pInfo->m_value.m_pszResult = m_current.row.c_str();
			pInfo->m_len = m_current.row.size();
			return;
	}
	if(index == m_pTableInfo->getColumnCount() + 1) 
	{
			//TIMESTAMP
			if(!m_current.columns.empty())
			{
				TCell& cell = m_current.columns.begin()->second;
				pInfo->m_value.m_lResult = cell.timestamp /1000;
			}
			else
			{
				pInfo->m_value.m_lResult = 0;
			}
			return;
	}
	DBColumnInfo* pColumn = m_pTableInfo->getColumn(index);
	if(pColumn->m_iKeyIndex >= 0)
	{
		const char* pszRowkey = m_current.row.c_str();
		size_t iKeyLen = m_current.row.size();
		int iKeyStart = 0;
		for(int i = 0; i < pColumn->m_iKeyIndex; ++i)
		{
			DBColumnInfo* pKey = m_pTableInfo->getKeyColumn(i);
			assert(pKey && pKey->m_iLen > 0);
			iKeyStart += pKey->m_iLen;
		}
		if(iKeyStart > iKeyLen)
		{
			throw new ExecutionException("rowkey is too short",false);
		}
		switch (pColumn->m_type)
		{
		case TYPE_INT64:
		case TYPE_INT32:
		case TYPE_INT16:
		case TYPE_INT8:
			{
				if(iKeyStart + pColumn->m_iLen > iKeyLen)
				{
					throw new ExecutionException("rowkey is too short",false);
				}
				uint64_t iValue = 0;
				char* pszValue = (char*)&iValue;
				const char* pszTarget = pszRowkey + iKeyStart;
				for(int i = 0;i<pColumn->m_iLen;++i)
				{
					pszValue[i] = pszTarget[pColumn->m_iLen - i - 1];
				}
				pInfo->m_value.m_lResult = iValue;
			}
			break;
		case TYPE_STRING:
		case TYPE_BYTES:
			pInfo->m_value.m_pszResult = pszRowkey + iKeyStart;
			if(pColumn->m_iKeyIndex  + 1 < m_pTableInfo->getKeyCount())
			{
				pInfo->m_len = pColumn->m_iLen;
			}
			else
			{
				pInfo->m_len = iKeyLen - iKeyStart;
			}
			break;
		default:
			assert(0);
			break;
		}
		return;
	}

	std::string s= m_cf;
	s+= pColumn->getName();
	std::string& value = m_current.columns[s].value;

	pInfo->m_bNull = false;
	switch (pColumn->m_type)
	{
	case TYPE_INT64:
	case TYPE_INT32:
	case TYPE_INT16:
	case TYPE_INT8:
		{
			if(value.size() != pColumn->m_iLen)
			{
				throw new ExecutionException("wrong return result",false);
			}
			uint64_t iValue = 0;
			char* pszValue = (char*)&iValue;
			const char* pszTarget = value.c_str();
			for(int i = 0;i<pColumn->m_iLen;++i)
			{
				pszValue[i] = pszTarget[pColumn->m_iLen - i - 1];
			}
			pInfo->m_value.m_lResult = iValue;
		}
		break;
	case TYPE_STRING:
	case TYPE_BYTES:
		pInfo->m_value.m_pszResult = value.c_str();
		pInfo->m_len = value.size();
		break;
	default:
		assert(0);
		break;
	}
}

int HBaseScanPlan::addProjection(ParseNode* pColumn)
{
	if (pColumn == NULL || pColumn->m_iType != NAME_NODE)
		return -1;

	if (Tools::isRowKeyNode(pColumn))
	{
		return m_pTableInfo->getColumnCount();
	}
	else if (Tools::isTimestampNode(pColumn))
	{
		return m_pTableInfo->getColumnCount() + 1;
	}
	else if(pColumn->m_iType == NAME_NODE)
	{
		for(size_t i=0;i<m_pTableInfo->getColumnCount();++i)
		{
			DBColumnInfo* pDBColumn = m_pTableInfo->getColumn(i);
			if(strcmp(pDBColumn->getName() , pColumn->m_pszValue) == 0)
			{
				if(pDBColumn->m_iKeyIndex >= 0) return i;
				std::string hcolumn = m_cf + pDBColumn->getName();
				for(size_t j=0;j<m_valueColumns.size();++j)
				{
					if(m_valueColumns[j] == hcolumn)
					{
						return i;
					}
				}
				m_valueColumns.push_back(hcolumn);
				return i;
			}
		}
		return -1;
	}
	else
	{
		LOG(ERROR, "unsupported hbase projection %s", pColumn->m_pszExpr);
		throw new ParseException("unsupported hbase projection %s", pColumn->m_pszExpr);
	}
}

void HBaseScanPlan::getAllColumns(std::vector<const char*>& columns)
{
	m_valueColumns.clear();
	for(size_t i=0;i<m_pTableInfo->getColumnCount();++i)
	{
		DBColumnInfo* pColumn = m_pTableInfo->getColumn(i);
		columns.push_back(pColumn->getName());
		if(pColumn->m_iKeyIndex < 0)
		{
			m_valueColumns.push_back(m_cf + pColumn->getName());
		}
	}
}

bool HBaseScanPlan::ensureSortOrder(size_t iSortIndex, const char* pszColumn, bool* pOrder)
{
	if (m_pTableInfo->getKeyCount() <= iSortIndex)
		return false;

	if (pOrder != NULL && !*pOrder) return false;

	DBColumnInfo* pKeyColumn = m_pTableInfo->getKeyColumn(iSortIndex);
	if (strcmp(pszColumn, pKeyColumn->getName()) != 0)
		return false;

	return true;
}
