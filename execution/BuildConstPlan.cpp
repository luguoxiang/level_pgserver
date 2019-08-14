#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "execution/ConstPlan.h"

void buildPlanForConst(ParseNode* pNode) {
	ConstPlan* pPlan = new ConstPlan();
	Tools::pushPlan(pPlan);
	ParseNode* pLastRow = nullptr;
	for (auto pRow:pNode->m_children) {
		if (pLastRow != nullptr && pLastRow->children() != pRow->children()) {
			throw new ParseException(
					"Values column number is not matched: %d is expected, but is %d!",
					pLastRow->children(), pRow->children());
		}
		pPlan->addRow(pRow);
		pLastRow = pRow;
	}
}

