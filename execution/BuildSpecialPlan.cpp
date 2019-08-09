#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include "execution/WorkloadResult.h"
#include "execution/ShowTables.h"
#include "execution/ShowColumns.h"
#include "execution/ReadFilePlan.h"

void buildPlanForLeftJoin(ParseNode* pNode)
{
}

void buildPlanForDesc(ParseNode* pNode)
{
	assert(pNode->m_iChildNum == 1);
	ParseNode* pTable = pNode->m_children[0];

	TableInfo* pEntry = NULL;
	if (pTable->m_iType != NAME_NODE)
	{
		assert(pTable->m_iChildNum == 2);

		ParseNode* pDB = pTable->m_children[0];
		pTable = pTable->m_children[1];

		assert(pDB->m_iType == NAME_NODE);
		assert(pTable->m_iType == NAME_NODE);

		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	} else {
		pEntry = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	}
	if (pEntry == NULL) {
		PARSE_ERROR("Undefined table %s", pTable->m_pszValue);
	}
	Tools::pushPlan(new ShowColumns(pEntry));
}

void buildPlanForWorkload(ParseNode* pNode)
{
	Tools::pushPlan(new WorkloadResult());
}

void buildPlanForShowTables(ParseNode* pNode)
{
	Tools::pushPlan(new ShowTables());
}

void buildPlanForReadFile(ParseNode* pNode)
{
	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pNode->m_children[0]->m_pszValue, //path
			pNode->m_children[3]->m_pszValue); //seperator
	Tools::pushPlan(pValuePlan);

	ParseNode* pTable = pNode->m_children[1];
	assert(pTable && pTable->m_iType == NAME_NODE);
	
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	if(pTableInfo == NULL)
	{
		throw new ParseException("Table %s does not exist!", pTable->m_pszValue);
	}

	ParseNode* pColumn = pNode->m_children[2];
	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(pColumn, columns);
	for(int i=0;i<columns.size();++i)
	{
			pValuePlan->addColumn(columns[i]);
	}
}

void buildPlanForFileSelect(ParseNode* pNode)
{
	assert(pNode && pNode->m_iChildNum == 7);

	ParseNode* pTable = pNode->m_children[SQL_SELECT_TABLE];
	assert(pTable && pTable->m_iType == NAME_NODE);
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	if (pTableInfo == NULL)
	{
		LOG(ERROR, "Undefined hbase table %s", pTable->m_pszValue);
		throw new ParseException("Undefined hbase table %s", pTable->m_pszValue);
	}

	ReadFilePlan* pValuePlan = new ReadFilePlan(
			pTableInfo->getAttribute("path").c_str(), 
			atoi(pTableInfo->getAttribute("seperator").c_str()));
	Tools::pushPlan(pValuePlan);

	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(NULL, columns);
	for(int i=0;i<columns.size();++i)
	{
			pValuePlan->addColumn(columns[i]);
	}
	BUILD_PLAN(pNode->m_children[SQL_SELECT_PREDICATE]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_GROUPBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_HAVING]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_ORDERBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_LIMIT]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_PROJECT]);
}
