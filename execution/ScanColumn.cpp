#include "ScanColumnImpl.h"
#include "ParseTools.h"
#include "common/Log.h"
#include "ExecutionException.h"
#include "common/ParseException.h"
#include "execution/WorkThreadInfo.h"

ScanColumn::ScanColumn(bool bProject, const char* pszBase, size_t id)
		: m_bProject(bProject)
{
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
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
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
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

