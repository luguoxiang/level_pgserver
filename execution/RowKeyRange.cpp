#include "RowKeyRange.h"
#include "common/SqlParser.tab.h"
#include "common/ParseException.h"

RowKeyRange::RowKeyRange() :
		m_pszRowKey1(0), m_pszRowKey2(0), m_iLen1(0), m_iLen2(0), m_bStartInc(
				0), m_bEndInc(0) {
}

RowKeyRange::~RowKeyRange() {
}

void RowKeyRange::parse(ParseNode* pNode) {
	if (pNode->m_iChildNum != 2) {
		m_conditions.push_back(pNode);
		return;
	}
	ParseNode* pLeft = pNode->m_children[0];
	ParseNode* pRight = pNode->m_children[1];
	int iOpCode = OP_CODE(pNode);
	int iReverse = 0;
	switch (OP_CODE(pNode)) {
	case ANDOP:
		parse(pLeft);
		parse(pRight);
		return;
	case COMP_GE:
		iReverse = COMP_LE;
		break;
	case COMP_GT:
		iReverse = COMP_LT;
		break;
	case COMP_LE:
		iReverse = COMP_GE;
		break;
	case COMP_LT:
		iReverse = COMP_GT;
		break;
	case COMP_EQ:
		iReverse = COMP_EQ;
		break;
	default:
		m_conditions.push_back(pNode);
		return;
	}
	bool bOptimized = false;
	if (pLeft->m_iType == NAME_NODE) {
		bOptimized = parseExpression(iOpCode, pLeft, pRight, pNode);
	} else if (pRight->m_iType == NAME_NODE) {
		bOptimized = parseExpression(iReverse, pRight, pLeft, pNode);
	}
	if (!bOptimized) {
		m_conditions.push_back(pNode);
	}
}
