#include "common/Log.h"
#include "common/ParseException.h"
#include "common/BuildPlan.h"
#include "common/MetaConfig.h"
#include "execution/HBaseScanPlan.h"
#include "execution/HBaseDeletePlan.h"
#include "execution/HBaseInsertPlan.h"
#include "execution/FilterPlan.h"
#include "execution/ExplicitRowKeyRange.h"
#include "execution/ImplicitRowKeyRange.h"

static void buildHBasePredicate(HBaseScanPlan* pPlan, ParseNode* pPredicate)
{
	if(pPredicate == NULL) return;

	TableInfo* pTableInfo = pPlan->getTableInfo();
	std::auto_ptr<RowKeyRange> pRowKeyRange;

	if (Tools::hasRowKey(pPredicate) || pTableInfo->getKeyCount() == 0)
	{
		pRowKeyRange.reset(new ExplicitRowKeyRange());
	}
	else
	{
		pRowKeyRange.reset(new ImplicitRowKeyRange(pTableInfo));
	}
	pRowKeyRange->parse(pPredicate);
	pRowKeyRange->done();

	if(!pRowKeyRange->isStartInclusive())
	{
			LOG(ERROR, "not inclusive rowkey left border is not supported");
			throw new ParseException("not inclusive rowkey left border is not supported");
	}
	if(pRowKeyRange->getConditionNum() > 0)
	{
			FilterPlan* pFilter(new FilterPlan(Tools::popPlan()));
			Tools::pushPlan(pFilter);
			for (size_t i = 0; i < pRowKeyRange->getConditionNum(); ++i)
			{
				ParseNode* pNode = pRowKeyRange->getCondition(i);
				pFilter->addPredicate(pNode);
			}
	}
	pPlan->setRowKeyRange(pRowKeyRange.release());
}

void buildPlanForHBaseInsert(ParseNode* pNode)
{
#ifdef UPDATE_SUPPORT
	assert(pNode && pNode->m_iChildNum >= 3);
	ParseNode* pTable = pNode->m_children[0];

	assert(pTable && pTable->m_iType == NAME_NODE);

	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	if(pTableInfo == NULL)
	{
		throw new ParseException(
					"Table %s does not exist!", pTable->m_pszValue);
	}

	HBaseInsertPlan* pPlan = new HBaseInsertPlan(pTableInfo);
	Tools::pushPlan(pPlan);

	ParseNode* pColumn = pNode->m_children[1];
	std::vector<DBColumnInfo*> columns;
	pTableInfo->getDBColumns(pColumn, columns);
	for (size_t j = 0; j < columns.size(); ++j)
	{
			pPlan->addColumn(columns[j]);
	}

	BUILD_PLAN(pNode->m_children[2]);

	pPlan->setValuePlan(Tools::popPlan());
#else
	PARSE_ERROR("Insert is not supported");
#endif
}


void buildPlanForHBaseSelect(ParseNode* pNode)
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
	HBaseScanPlan* pPlan = new HBaseScanPlan(pTableInfo);
	Tools::pushPlan(pPlan);
	buildHBasePredicate(pPlan, pNode->m_children[SQL_SELECT_PREDICATE]);

	BUILD_PLAN(pNode->m_children[SQL_SELECT_GROUPBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_HAVING]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_ORDERBY]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_LIMIT]);
	BUILD_PLAN(pNode->m_children[SQL_SELECT_PROJECT]);
}

void buildPlanForHBaseDelete(ParseNode* pNode)
{
#ifdef UPDATE_SUPPORT
	assert(pNode && pNode->m_iChildNum >= 2);
	ParseNode* pTable = pNode->m_children[0];

	assert(pTable);
	if (pTable->m_iType != NAME_NODE)
	{
		throw new ParseException(
				"Delete table with database specified is not supported.");
	}
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	if (pTableInfo == NULL)
	{
		LOG(ERROR, "Undefined table %s", pTable->m_pszValue);
		throw new ParseException("Undefined table %s", pTable->m_pszValue);
	}


	HBaseScanPlan* pSearchPlan = new HBaseScanPlan(pTableInfo);
	
	Tools::pushPlan(pSearchPlan);
	buildHBasePredicate(pSearchPlan, pNode->m_children[1]);

	HBaseDeletePlan* pPlan = new HBaseDeletePlan(pTableInfo, Tools::popPlan());
	Tools::pushPlan(pPlan);
#else
	PARSE_ERROR("Delete is not supported");
#endif
}
