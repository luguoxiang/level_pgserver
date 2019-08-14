#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ShowColumns.h"
#include "execution/ReadFilePlan.h"

void buildPlanForLeftJoin(ParseNode* pNode) {
}

void buildPlanForDesc(ParseNode* pNode) {
	assert(pNode->children() == 1);
	ParseNode* pTable = pNode->m_children[0];

	TableInfo* pEntry = nullptr;
	if (pTable->m_type != NodeType::NAME) {
		assert(pTable->children() == 2);

		ParseNode* pDB = pTable->m_children[0];
		pTable = pTable->m_children[1];

		assert(pDB->m_type == NodeType::NAME);
		assert(pTable->m_type == NodeType::NAME);

		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	} else {
		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_sValue);
	}
	if (pEntry == nullptr) {
		PARSE_ERROR("Undefined table %s", pTable->m_sValue.c_str());
	}
	Tools::pushPlan(new ShowColumns(pEntry));
}

void buildPlanForWorkload(ParseNode* pNode) {
	Tools::pushPlan(new WorkloadResult());
}

void buildPlanForShowTables(ParseNode* pNode) {
	Tools::pushPlan(new ShowTables());
}

void buildPlanForReadFile(ParseNode* pNode) {
	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pNode->m_children[0]->m_sValue, //path
			pNode->m_children[3]->m_sValue); //seperator
	Tools::pushPlan(pValuePlan);

	ParseNode* pTable = pNode->m_children[1];
	assert(pTable && pTable->m_type == NodeType::NAME);

	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(
			pTable->m_sValue);
	if (pTableInfo == nullptr) {
		throw new ParseException("Table %s does not exist!",
				pTable->m_sValue.c_str());
	}

	ParseNode* pColumn = pNode->m_children[2];
	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(pColumn, columns);
	for (auto p: columns) {
		pValuePlan->addColumn(p);
	}
}

void buildPlanForFileSelect(ParseNode* pNode) {
	assert(pNode && pNode->children() == 7);

	ParseNode* pTable = pNode->m_children[SQL_SELECT_TABLE];
	assert(pTable && pTable->m_type == NodeType::NAME);
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(
			pTable->m_sValue);
	if (pTableInfo == nullptr) {
		LOG(ERROR, "Undefined hbase table %s", pTable->m_sValue.c_str());
		throw new ParseException("Undefined hbase table %s",
				pTable->m_sValue.c_str());
	}

	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path"),
			pTableInfo->getAttribute("seperator"));
	Tools::pushPlan(pValuePlan);

	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(nullptr, columns);
	for (auto p:columns) {
		pValuePlan->addColumn(p);
	}
	BUILD_PLAN(pNode->m_children[SQL_SELECT_PREDICATE]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_GROUPBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_HAVING]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_ORDERBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_LIMIT]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_PROJECT]);
}
