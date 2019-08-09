#pragma once
#include "RowKeyRange.h"
class ExplicitRowKeyRange: public RowKeyRange {
public:
	ExplicitRowKeyRange();
	virtual ~ExplicitRowKeyRange();

	virtual void done() {
	}

	virtual bool isRangeSearch() {
		return m_bRangeSearch;
	}
	ExplicitRowKeyRange(const ExplicitRowKeyRange&) = delete;
	ExplicitRowKeyRange& operator =(const ExplicitRowKeyRange&) = delete;

protected:
	virtual bool parseExpression(int iOpCode, ParseNode* pLeft,
			ParseNode* pRight, ParseNode* pExpr);

private:
	void setRowKey(const char*& pszRowKey, int64_t &m_iLen, ParseNode* pNode);

	bool m_bRangeSearch;
};

