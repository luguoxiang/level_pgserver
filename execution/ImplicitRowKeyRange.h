#pragma once
#include "RowKeyRange.h"
#include <string>
#include <vector>
#include "common/ConfigInfo.h"

class ImplicitRowKeyRange: public RowKeyRange {
public:
	ImplicitRowKeyRange(TableInfo* pTableInfo);
	~ImplicitRowKeyRange();

	virtual void done();
public:
	//used to create rowkey from column values in insert statement
	void setColumnValue(DBColumnInfo* pColumn, ParseNode* pValue);

	virtual bool isRangeSearch();

	ImplicitRowKeyRange(const ImplicitRowKeyRange&) = delete;
	ImplicitRowKeyRange& operator =(const ImplicitRowKeyRange&) = delete;

protected:
	virtual bool parseExpression(int iOpCode, ParseNode* pLeft,
			ParseNode* pRight, ParseNode* pExpr);
private:
	void evaluate();
	struct RowKeyField {
		ParseNode* m_pStartValue;
		bool m_bStartInclusive;

		ParseNode* m_pEndValue;
		bool m_bEndInclusive;

		//store the expression of >=, =, >
		ParseNode* m_pExpr1;

		//store the expression of <, <=
		ParseNode* m_pExpr2;
	};

	void setRowKeyFieldData(char* pszRowKey, int64_t iLen, ParseNode* pNode,
			const char* pszColumn);

	std::vector<RowKeyField> m_fields;

	size_t m_iRowkeySearchCount;

	TableInfo* m_pTableInfo;
};

