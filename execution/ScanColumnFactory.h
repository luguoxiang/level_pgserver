#pragma once

#include "ParseTools.h"
#include "ExecutionPlan.h"
#include "ScanColumn.h"
#include "ScanColumnImpl.h"
#include "RowKeyRange.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <stdio.h>
#include <stdarg.h>

class ScanColumnFactory {
	friend class OBScanPlan;
public:
	ScanColumnFactory() {
	}

	~ScanColumnFactory() {
	}

	void clear();

	ScanColumn* addJoinColumn(const char* pszKey, const char* pszTable,
			const char* pszForeignColumn);

	ScanColumn* addScanColumn(const char* pszName, bool bProject);

	ScanColumn* addScanComplexColumn(const char* pszExpr,
			const char* pszRawExpr, bool bProject, bool bSimple);
	ScanColumn* addFunctionColumn(const char* pszFuncName,
			const char* pszRawExpr, const char* sArg, bool bProject);

	ScanColumn* addConstValue(const char* pszValue);

	ScanColumn* getScanColumnInfo(const char*, bool bProject);

	ScanColumn* getAliasColumnInfo(const char*);

	void alias(const char* pszName, ScanColumn* pColumn) {
		m_aliasMap[pszName] = pColumn;
	}
private:
	ScanColumnVector m_scanColumns;
	ScanColumnVector m_paramColumns;
	ScanColumnVector m_exprColumns;
	ScanColumnVector m_aggrColumns;
	ScanColumnVector m_aggrExprColumns;
	ScanColumnVector m_joinColumns;
	ScanColumnVector m_constColumns;

	/*
	 * map from column name to ScanColumn information
	 * for example:
	 * select sum(item_price) from collect_item groupby item_name;
	 * sum(item_price) will be named as aggr_0 for ob_aggregate_column, so
	 * m_columnMap["_aggr_0"] = pScanColumn; (by addScanAggregateColumn method)
	 * m_columnMap["sum item_price"] = pScanColumn; (by addScanAggregateColumn method)
	 * 
	 * '_aggr_0' is used to find ScanColumn from query return value(OB_CELL->column)
	 * 'sum item_price' entry is used to avoid generating two ScanColumn for same expression,
	 *                continue above example: select ... having sum(item_price) > 0.
	 */
	std::map<std::string, ScanColumn*> m_columnMap;

	std::map<std::string, ScanColumn*> m_aliasMap;

};

