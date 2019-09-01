
#include "common/MetaConfig.h"
#include "common/ParseException.h"

#include "execution/ParseTools.h"
#include "execution/ShowColumns.h"
#include "execution/BuildPlan.h"
#include "execution/UnionAllPlan.h"

ExecutionPlanPtr buildPlanForDesc(const ParseNode* pNode) {
	assert(pNode->children() == 1);
	auto pTable = pNode->getChild(0);

	const TableInfo* pEntry = nullptr;
	if (pTable->m_type != NodeType::NAME) {
		assert(pTable->children() == 2);

		auto pDB = pTable->getChild(0);
		pTable = pTable->getChild(1);

		assert(pDB->m_type == NodeType::NAME);
		assert(pTable->m_type == NodeType::NAME);

		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	} else {
		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	}
	if (pEntry == nullptr) {
		PARSE_ERROR("Undefined table ", pTable->m_sValue);
	}

	return ExecutionPlanPtr(new ShowColumns(pEntry));
}




ExecutionPlanPtr buildPlanForUnionAll(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 2);
	ExecutionPlanPtr pLeft = buildPlan(pNode->getChild(0));
	ExecutionPlanPtr pRight = buildPlan(pNode->getChild(1));
	assert(pLeft && pRight);

	UnionAllPlan* pPlan = new UnionAllPlan(false);
	ExecutionPlanPtr pResult(pPlan);

	pPlan->addChildPlan(pLeft.release());
	pPlan->addChildPlan(pRight.release());


	int count = pLeft->getResultColumns();
	if (count != pRight->getResultColumns()) {
		PARSE_ERROR(
				"left sub query's column number is not same with right one's!");
	}
	for (int i = 0; i < count; ++i) {
		DBDataType type1 = pLeft->getResultType(i);
		DBDataType type2 = pRight->getResultType(i);

		switch (type1) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			type1 = DBDataType::INT64;
			break;
		default:
			break;
		}

		switch (type2) {
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			type2 = DBDataType::INT64;
			break;
		default:
			break;
		}

		if (type1 != type2) {
			PARSE_ERROR("sub query column %d's type are not match");
		}
	}
	return pResult;
}


