#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "execution/ConstPlan.h"

void buildPlanForConst(ParseNode* pNode) {
	ConstPlan* pPlan = new ConstPlan();
	Tools::pushPlan(pPlan);
	ParseNode* pLastRow = NULL;
	for (size_t i = 0; i < pNode->m_iChildNum; ++i) {
		ParseNode* pRow = pNode->m_children[i];
		if (pLastRow != NULL && pLastRow->m_iChildNum != pRow->m_iChildNum) {
			throw new ParseException(
					"Values column number is not matched: %d is expected, but is %d!",
					pLastRow->m_iChildNum, pRow->m_iChildNum);
		}
		pPlan->addRow(pRow);
		pLastRow = pRow;
	}
}

