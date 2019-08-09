#pragma once

#include <assert.h>
#include <vector>
#include <stdint.h>
#include "common/ParseNode.h"

class RowKeyRange
{
public:
	RowKeyRange();
	virtual ~RowKeyRange();

	const char* getStartRowKey()
	{
		return m_pszRowKey1;
	}

	const char* getEndRowKey()
	{
		return m_pszRowKey2;
	}

	int64_t getStartLength()
	{
		return m_iLen1;
	}

	int64_t getEndLength()
	{
		return m_iLen2;
	}

	bool isStartInclusive()
	{
		return m_bStartInc;
	}

	bool isEndInclusive()
	{
		return m_bEndInc;
	}

	size_t getConditionNum()
	{
		return m_conditions.size();
	}

	ParseNode* getCondition(size_t i)
	{
		assert(i>=0 && i< m_conditions.size());
		return m_conditions[i];
	}

	//called during plan building
	void parse(ParseNode* pNode);

	//called during plan building
	virtual void done() = 0;

	virtual bool isRangeSearch() = 0;

	RowKeyRange(const RowKeyRange&) = delete;
	RowKeyRange& operator =(const RowKeyRange&) = delete;
protected:
	virtual bool parseExpression(int iOpCode, ParseNode* pLeft,
			ParseNode* pRight, ParseNode* pExpr) = 0;

	const char* m_pszRowKey1;
	const char* m_pszRowKey2;

	int64_t m_iLen1;
	int64_t m_iLen2;

	bool m_bStartInc;
	bool m_bEndInc;
	std::vector<ParseNode*> m_conditions;
};

