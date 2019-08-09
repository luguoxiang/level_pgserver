#pragma once

#include "ParseTreeVisitor.h"
#include "ScanColumn.h"
#include "ScanColumnFactory.h"
#include <string>

/*
 * Generate a ScanColumn object for the expression parse tree.
 * ParseNode::m_pszExpr is used to eliminate duplicate expression.
 * for example: sum(a) > 0 in select sum(a) from t groupby b having sum(a) > 0
 *	
 */
class BuildExpressionVisitor: public ParseTreeVisitor<bool, ScanColumn*> {
public:
	BuildExpressionVisitor(ScanColumnFactory* pFactory, TableInfo* pTableInfo,
			bool bGroupBy) :
			m_pFactory(pFactory), m_pTableInfo(pTableInfo), m_bGroupBy(bGroupBy) {
	}

	static ScanColumn* addSimpleScanColumn(const char* pszName,
			ScanColumnFactory* pFactory, DBColumnInfo* pColumnInfo,
			bool bProject);

	virtual ScanColumn* visitDataNode(ParseNode* pNode, bool);

	virtual ScanColumn* visitNameNode(ParseNode* pNode, bool bProject);

	virtual ScanColumn* visitDyadicOpNode(int op, ParseNode* pNode,
			bool bProject);

	virtual ScanColumn* visitUnaryOpNode(int op, ParseNode* pNode,
			bool bProject);

	virtual ScanColumn* visitFuncNode(const char* pszName, ParseNode* pParam,
			bool bProject);

private:
	ScanColumnFactory* m_pFactory;
	TableInfo* m_pTableInfo;
	bool m_bGroupBy;
};
