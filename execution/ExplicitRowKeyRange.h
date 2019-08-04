#ifndef EXPLICITROWKEYRANGE_H
#define EXPLICITROWKEYRANGE_H
#include "RowKeyRange.h"
class ExplicitRowKeyRange: public RowKeyRange
{
public:
	ExplicitRowKeyRange();
	virtual ~ExplicitRowKeyRange();

	virtual void done()
	{
	}

	virtual bool isRangeSearch()
	{
		return m_bRangeSearch;
	}
protected:
	virtual bool parseExpression(int iOpCode, ParseNode* pLeft,
			ParseNode* pRight, ParseNode* pExpr);

private:
	ExplicitRowKeyRange(const ExplicitRowKeyRange&);
	ExplicitRowKeyRange& operator =(const ExplicitRowKeyRange&);

	void setRowKey(const char*& pszRowKey, int64_t &m_iLen, ParseNode* pNode);

	bool m_bRangeSearch;
};

#endif //EXPLICITROWKEYRANGE_H
