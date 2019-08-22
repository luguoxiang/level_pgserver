#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "execution/ConstPlan.h"

void buildPlanForConst(const ParseNode* pNode) {
	ConstPlan* pPlan = new ConstPlan();
	Tools::pushPlan(pPlan);
	const ParseNode* pLastRow = nullptr;
	for (size_t i=0;i<pNode->children(); ++i) {
		auto pRow = pNode->getChild(i);
		if (pLastRow != nullptr && pLastRow->children() != pRow->children()) {
			PARSE_ERROR("Values column number does not match: expect ", pLastRow->children(), " but got ", pRow->children());
		}
		pPlan->addRow(pRow);
		pLastRow = pRow;
	}
}

