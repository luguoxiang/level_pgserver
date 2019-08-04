#include "ScanColumnImpl.h"
#include "ParseTools.h"
#include "common/Log.h"
#include "ExecutionException.h"
#include "common/ParseException.h"
#include "execution/WorkThreadInfo.h"

ScanColumn::ScanColumn(bool bProject, const char* pszBase, size_t id)
		: m_bProject(bProject), m_pCell(0)
{
	WorkThreadInfo* pInfo = (WorkThreadInfo*) pthread_getspecific(
			WorkThreadInfo::tls_key);
	assert(pInfo);
	size_t len = strlen(pszBase) + 10;
	char* pszName = pInfo->alloc(len);
	snprintf(pszName, len, "_%s_%ld", pszBase, id);
	setName(pszName);
}

JoinColumn::JoinColumn(const char* pszColumn, const char* pszTable,
		const char* pszForeignColumn)
		: ScanColumn(true, ""), m_pszColumn(pszColumn), m_pszTable(pszTable), m_pszForeignColumn(
				pszForeignColumn)
{
	WorkThreadInfo* pInfo = (WorkThreadInfo*) pthread_getspecific(
			WorkThreadInfo::tls_key);
	assert(pInfo);
	size_t len = strlen(pszTable) + strlen(pszForeignColumn) + 2;
	char* pszName = pInfo->alloc(len);
	snprintf(pszName, len, "%s.%s", pszTable, pszForeignColumn);
	setName(pszName);
}

std::string SimpleScanColumn::explain()
{
	std::string s = "ob:scan_column ";
	s.append(getName());
	if (m_bProject)
	{
		s.append(", projected");
	}
	return s;
}

void SimpleScanColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	LOG(DEBUG, "ob:scan_column: name=%s, projection=%d", getName(), m_bProject);
	OB_ERR_CODE err = ob_scan_column(pScan, getName(), m_bProject);
	CHECK_ERROR(err, "Failed to set projection columns");
}

std::string ExprColumn::explain()
{
	std::string s = "ob:scan_complex_column ";
	s.append(m_pszExpr);
	s.append(",");
	s.append(getName());
	if (m_bProject)
	{
		s.append(", projected");
	}
	return s;
}

void ExprColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	LOG(DEBUG, "ob:scan_complex_column: expr=%s, name=%s, projection=%d",
			m_pszExpr, getName(), m_bProject);

	OB_ERR_CODE err = ob_scan_complex_column(pScan, m_pszExpr, getName(),
			m_bProject);
	CHECK_ERROR(err, "Failed to set complex columns");
}

std::string GroupByExprColumn::explain()
{
	std::string s = "ob:groupby_add_complex_column ";
	s.append(m_pszExpr);
	s.append(",");
	s.append(getName());
	if (m_bProject)
	{
		s.append(", projected");
	}
	return s;
}

void GroupByExprColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	assert(pGroupBy);
	LOG(DEBUG, "ob:groupby_add_complex_column: expr=%s, name=%s, projection=%d",
			m_pszExpr, getName(), m_bProject);

	OB_ERR_CODE err = ob_groupby_add_complex_column(pGroupBy, m_pszExpr,
			getName(), m_bProject);
	CHECK_ERROR(err, "Failed to set groupby complex columns");
}

std::string GroupByReturnColumn::explain()
{
	std::string s = "ob:groupby_add_return_column";
	s.append(getName());
	if (m_bProject)
	{
		s.append(", projected");
	}
	return s;
}

void GroupByReturnColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	LOG(DEBUG, "ob:ob_groupby_add_return_column:name=%s, projection=%d",
			getName(), m_bProject);
	OB_ERR_CODE err = ob_groupby_add_return_column(pGroupBy, getName(),
			m_bProject);
	CHECK_ERROR(err, "Failed to set aggregate column");
}

std::string AggrColumn::explain()
{
	std::string s = "ob:aggregate_column ";
	s.append(m_pszFuncName);
	s.append("(");
	s.append(m_pszArg);
	s.append("), ");
	s.append(getName());
	if (m_bProject)
	{
		s.append(", projected");
	}
	return s;
}

void AggrColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	LOG(DEBUG, "ob:aggregate_column:func=%s, arg=%s, name=%s, projection=%d",
			m_pszFuncName, m_pszArg, getName(), m_bProject);
	assert(pGroupBy);
	OB_AGGREGATION_TYPE iOBFunc;
	if (strcmp(m_pszFuncName, "count") == 0)
	{
		iOBFunc = OB_COUNT;
	}
	else if (strcmp(m_pszFuncName, "sum") == 0)
	{
		iOBFunc = OB_SUM;
	}
	else if (strcmp(m_pszFuncName, "min") == 0)
	{
		iOBFunc = OB_MIN;
	}
	else if (strcmp(m_pszFuncName, "max") == 0)
	{
		iOBFunc = OB_MAX;
	}
	else
	{
		throw new ParseException("Unsupported function '%s'", m_pszFuncName);
	}

	OB_ERR_CODE err = ob_aggregate_column(pGroupBy, iOBFunc, m_pszArg,
			getName(), m_bProject);
	CHECK_ERROR(err, "Failed to set aggregate column");
}

std::string JoinColumn::explain()
{
	std::string s = "ob:res_join_append '";
	s.append(m_pszColumn);
	s.append("',");
	s.append(m_pszTable);
	s.append(",");
	s.append(m_pszForeignColumn);
	return s;
}

void JoinColumn::add(OB_SCAN* pScan, OB_GROUPBY_PARAM* pGroupBy)
{
	LOG(DEBUG, "ob:res_join_append:column=%s, table=%s, foreign=%s, expr=%s",
			m_pszColumn, m_pszTable, m_pszForeignColumn, getName());
	OB_ERR_CODE err = ob_scan_res_join_append(pScan, m_pszColumn, m_pszTable,
			m_pszForeignColumn, getName());
	CHECK_ERROR(err, "Failed to set join column");
}
