#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ShowColumns.h"
#include "execution/ReadFilePlan.h"


void buildPlanForDesc(const ParseNode* pNode) {
	assert(pNode->children() == 1);
	auto pTable = pNode->getChild(0);

	TableInfo* pEntry = nullptr;
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
	Tools::pushPlan(new ShowColumns(pEntry));
}

void buildPlanForWorkload(const ParseNode* pNode) {
	Tools::pushPlan(new WorkloadResult());
}

void buildPlanForShowTables(const ParseNode* pNode) {
	Tools::pushPlan(new ShowTables());
}

void buildPlanForReadFile(const ParseNode* pNode) {
	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pNode->getChild(0)->m_sValue, //path
			pNode->getChild(3)->m_sValue); //seperator
	Tools::pushPlan(pValuePlan);

	auto pTable = pNode->getChild(1);
	assert(pTable && pTable->m_type == NodeType::NAME);

	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(
			pTable->m_sValue);
	if (pTableInfo == nullptr) {
		PARSE_ERROR("Table ", pTable->m_sValue, " does not exist!");
	}

	auto pColumn = pNode->getChild(2);
	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(pColumn, columns);
	for (auto p: columns) {
		pValuePlan->addColumn(p);
	}
}

void buildPlanForFileSelect(const ParseNode* pNode) {
	assert(pNode && pNode->children() == 7);

	const ParseNode* pTable = pNode->getChild(SQL_SELECT_TABLE);
	assert(pTable && pTable->m_type == NodeType::NAME);
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(
			pTable->m_sValue);
	if (pTableInfo == nullptr) {
		PARSE_ERROR("Table ", pTable->m_sValue," does not exist!");
	}

	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path"),
			pTableInfo->getAttribute("seperator"),
			Tools::case_equals(pTableInfo->getAttribute("ignore_first_line"), "true"));
	Tools::pushPlan(pValuePlan);

	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(nullptr, columns);
	for (auto p:columns) {
		pValuePlan->addColumn(p);
	}
	BUILD_PLAN(pNode->getChild(SQL_SELECT_PREDICATE));
	BUILD_PLAN(pNode->getChild(SQL_SELECT_GROUPBY));
	BUILD_PLAN(pNode->getChild(SQL_SELECT_HAVING));
	BUILD_PLAN(pNode->getChild(SQL_SELECT_ORDERBY));
	BUILD_PLAN(pNode->getChild(SQL_SELECT_LIMIT));
	BUILD_PLAN(pNode->getChild(SQL_SELECT_PROJECT));
}
