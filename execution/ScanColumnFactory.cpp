#include "ScanColumnFactory.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include "common/SqlParser.tab.h"
#include "common/Log.h"
#include "ScanColumnImpl.h"

ScanColumn* ScanColumnFactory::getAliasColumnInfo(const char* pszName) {
	auto iter = m_aliasMap.find(pszName);

	if (iter == m_aliasMap.end())
		return 0;

	return iter->second;
}

ScanColumn* ScanColumnFactory::getScanColumnInfo(const char* pszName,
		bool bProject) {

	auto iter = m_columnMap.find(pszName);

	if (iter == m_columnMap.end())
		return 0;

	ScanColumn* pColumn = iter->second;

	pColumn->m_bProject |= bProject;
	return pColumn;
}

ScanColumn*
ScanColumnFactory::addConstValue(const char* pszValue) {
	assert(pszValue);
	ScanColumn* pColumn = new ConstScanColumn(pszValue);
	m_constColumns.push_back(pColumn);
	return pColumn;
}

ScanColumn* ScanColumnFactory::addScanColumn(const char* pszName,
		bool bProject) {
	ScanColumn* pColumn = new SimpleScanColumn(pszName, bProject);

	m_scanColumns.push_back(pColumn);
	m_columnMap[pszName] = pColumn;
	return pColumn;
}

ScanColumn*
ScanColumnFactory::addFunctionColumn(const char* pszFuncName,
		const char* pszRawExpr, const char* pszArg, bool bProject) {
	ScanColumn* pColumn = NULL;
	if (strcmp(pszFuncName, "first") == 0) {
		pColumn = new GroupByReturnColumn(bProject, pszArg);
	} else {
		pColumn = new AggrColumn(bProject, pszFuncName, pszArg,
				m_aggrColumns.size());
	}

	m_aggrColumns.push_back(pColumn);
	m_columnMap[pColumn->getName()] = pColumn;
	m_columnMap[pszRawExpr] = pColumn;
	return pColumn;
}

ScanColumn*
ScanColumnFactory::addScanComplexColumn(const char* pszExpr,
		const char* pszRawExpr, bool bProject, bool bSimple) {
	ScanColumn* pColumn = NULL;
	if (bSimple) {
		pColumn = new ExprColumn(pszExpr, bProject, m_exprColumns.size());
		m_exprColumns.push_back(pColumn);
	} else {
		pColumn = new GroupByExprColumn(pszExpr, bProject,
				m_aggrExprColumns.size());
		m_aggrExprColumns.push_back(pColumn);
	}

	m_columnMap[pColumn->getName()] = pColumn;
	m_columnMap[pszRawExpr] = pColumn;
	return pColumn;
}

ScanColumn* ScanColumnFactory::addJoinColumn(const char* pszKey,
		const char* pszTable, const char* pszForeignColumn) {
	ScanColumn* pJoinColumn = new JoinColumn(pszKey, pszTable,
			pszForeignColumn);
	m_columnMap[pJoinColumn->getName()] = pJoinColumn;
	m_joinColumns.push_back(pJoinColumn);
	return pJoinColumn;
}

void ScanColumnFactory::clear() {
	m_scanColumns.clear();

	m_exprColumns.clear();

	m_aggrExprColumns.clear();

	m_aggrColumns.clear();

	m_joinColumns.clear();

	m_constColumns.clear();

	m_columnMap.clear();

}

