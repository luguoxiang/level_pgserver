#pragma once

#include "ScanColumn.h"
#include "common/ParseNode.h"

class ConstScanColumn: public ScanColumn {
public:
	ConstScanColumn(const char* pszValue) :
			ScanColumn(false, pszValue) {
	}

	virtual std::string explain() {
		return getName();
	}
};

//information for ob_scan_column
//	stored in m_scanColumns,m_sArg( == m_sName) is the column name
class SimpleScanColumn: public ScanColumn {
public:
	SimpleScanColumn(const char* pszName, bool bProject) :
			ScanColumn(bProject, pszName) {
	}

	virtual std::string explain();
};

//information for ob_scan_complex_column 
//	stored in m_simpleExprColumns 
class ExprColumn: public ScanColumn {
public:
	ExprColumn(const char* pszExpr, bool bProject, size_t id) :
			ScanColumn(bProject, "expr", id), m_pszExpr(pszExpr) {
	}
	virtual std::string explain();
private:
	const char* m_pszExpr;
};

//information for ob_groupby_add_complex_column 
//	stored in m_exprColumns 
class GroupByExprColumn: public ScanColumn {
public:
	GroupByExprColumn(const char* pszExpr, bool bProject, size_t id) :
			ScanColumn(bProject, "aggr_expr", id), m_pszExpr(pszExpr) {
	}
	virtual std::string explain();
private:
	const char* m_pszExpr;
};

//information for ob_aggregate_column(m_iType == FMIN,FMAX,FSUM,FCOUNT)
//	stored in m_exprColumns, m_sArg is argument, m_sName is column name
class AggrColumn: public ScanColumn {
public:
	AggrColumn(bool bProject, const char* pszName, const char* pszArg,
			size_t id) :
			ScanColumn(bProject, "aggr", id), m_pszFuncName(pszName), m_pszArg(
					pszArg) {
	}
	virtual std::string explain();

private:
	const char* m_pszFuncName;
	const char* m_pszArg;
};

//information for ob_groupby_add_return_column
//	stored in m_exprColumns, m_sArg is argument, m_sName is column name
class GroupByReturnColumn: public ScanColumn {
public:
	GroupByReturnColumn(bool bProject, const char* pszName) :
			ScanColumn(bProject, pszName) {
	}
	virtual std::string explain();
};

class JoinColumn: public ScanColumn {
public:
	JoinColumn(const char* pszColumn, const char* pszTable,
			const char* pszForeignColumn);

	virtual std::string explain();
private:
	const char* m_pszColumn;
	const char* m_pszTable;
	const char* m_pszForeignColumn;
};
