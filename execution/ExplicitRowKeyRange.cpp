#include "ExplicitRowKeyRange.h"
#include "common/ParseException.h"
#include "common/SqlParser.tab.h"
#include "ParseTools.h"

ExplicitRowKeyRange::ExplicitRowKeyRange() : m_bRangeSearch(false)
{
}
ExplicitRowKeyRange::~ExplicitRowKeyRange()
{
}

void ExplicitRowKeyRange::setRowKey(const char*& pszRowKey, int64_t &iLen,
		ParseNode* pNode)
{
	switch (pNode->m_iType)
	{
	case BINARY_NODE:
	case STR_NODE:
		if (iLen > 0 && iLen != pNode->m_iValue)
		{
			throw new ParseException("Conflict rowkey length!");
		}
		iLen = pNode->m_iValue;
		pszRowKey = pNode->m_pszValue;
		break;
	default:
		throw new ParseException(
				"unsupported rowkey type, use binary type instead!");
	}
}

bool ExplicitRowKeyRange::parseExpression(int iOpCode, ParseNode* pLeft,
		ParseNode* pRight, ParseNode* pExpr)
{
	assert(pLeft && pRight);
	if (!Tools::isRowKeyNode(pLeft))
		return false;

	switch (iOpCode)
	{
	case COMP_GE:
	case COMP_GT:
		if (m_pszRowKey1 != 0)
		{
			throw new ParseException("conflict row key range!");
		}
		setRowKey(m_pszRowKey1, m_iLen1, pRight);
		m_bStartInc = (iOpCode == COMP_GE);
		break;
	case COMP_LE:
	case COMP_LT:
		if (m_pszRowKey2 != 0)
		{
			throw new ParseException("conflict row key range!");
		}
		setRowKey(m_pszRowKey2, m_iLen2, pRight);
		m_bEndInc = (iOpCode == COMP_LE);
		break;
	case COMP_EQ:
		if (m_pszRowKey1 != 0 || m_pszRowKey2 != 0)
		{
			throw new ParseException("conflict row key range!");
		}
		setRowKey(m_pszRowKey1, m_iLen1, pRight);
		setRowKey(m_pszRowKey2, m_iLen2, pRight);
		m_bStartInc = true;
		m_bEndInc = true;
		break;
	default:
		assert(0);
		break;
	}
	return true;
}

