#include "common/SqlParser.tab.h"
#include "common/BuildPlan.h"
#include "OBScanPlan.h"
#include "common/ParseException.h"
#include "common/Log.h"
#include "ParseTools.h"
#include "ExplainPlan.h"
#include "BuildExpressionVisitor.h"
#include "execution/ExecutionException.h"
#include "common/MetaConfig.h"
#include "execution/ExplicitRowKeyRange.h"
#include "execution/ImplicitRowKeyRange.h"
#include "execution/OBScanPlan.h"
#include "execution/OBInsertPlan.h"
#include "execution/OBDeletePlan.h"

static ScanColumn* buildScanColumn(OBScanPlan* pPlan, ParseNode* pNode,
		TableInfo* pTableInfo, bool hasGroupBy)
{
	BuildExpressionVisitor expressionBuilder(pPlan->getColumnFactory(),
			pTableInfo, hasGroupBy);
	ScanColumn* pColumn = expressionBuilder.walk(pNode, false);
	return pColumn;
}

void buildOBPredicate(ParseNode* pPredicate)
{
	if(pPredicate == NULL) return;
	OBScanPlan* pPlan = (OBScanPlan*) Tools::popPlan();
	Tools::pushPlan(pPlan);

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

	std::string s;
	for (size_t i = 0; i < pRowKeyRange->getConditionNum(); ++i)
	{
		ParseNode* pNode = pRowKeyRange->getCondition(i);

		if (pNode->m_iType == BOOL_NODE)
		{
			if (pNode->m_iValue == 1)
				continue;
			if (pNode->m_iValue == 0)
			{
				LOG(ERROR, "false is not supported!");
				throw new ParseException("false is not supported!");
			}
		}
		else if (s.size() > 0)
		{
			s.append(" and ");
		}
		ScanColumn* pColumn = buildScanColumn(pPlan, pNode, pTableInfo, false);
		assert(pColumn);
		s.append(pColumn->getName());

	}
	pPlan->setWhere(s);
	pPlan->setRowKeyRange(pRowKeyRange.release());
}

static void buildHaving(OBScanPlan* pPlan, ParseNode* pNode,
		TableInfo* pTableInfo)
{
	if (pNode == 0)
		return;

	if (!pPlan->hasGroupBy())
	{
		LOG(ERROR, "Having clause without group by columns!");
		throw new ParseException("Having clause without group by columns!");
	}
	ScanColumn* pColumn = buildScanColumn(pPlan, pNode, pTableInfo, true);
	assert(pColumn);
	pPlan->setHaving(pColumn->getName());

}

static void buildLimit(OBScanPlan* pPlan, ParseNode* pNode)
{
	if (pNode == NULL)
		return;

	assert(pNode->m_iChildNum == 2);
	ParseNode* pCount = pNode->m_children[0];
	ParseNode* pOffset = pNode->m_children[1];

	assert(pCount->m_iType == INT_NODE);
	assert(pOffset->m_iType == INT_NODE);
	int64_t iOffset = pOffset->m_iValue;
	int64_t iCount = pCount->m_iValue;
	if (iCount == 0)
	{
		LOG(ERROR, "The limit number should be bigger than zero!");
		throw new ParseException(
				"The limit number should be bigger than zero!");
	}
	pPlan->setLimit(iOffset, iCount);
}

static void buildOrderBy(OBScanPlan* pGetTablePlan, ParseNode* pNode,
		TableInfo* pTableInfo)
{
	if (pNode == NULL)
		return;

	assert(pNode->m_iChildNum > 0);
	for (size_t i = 0; i < pNode->m_iChildNum; ++i)
	{
		ParseNode* pChild = pNode->m_children[i];
		if (pChild == 0)
			continue;

		assert(pChild->m_iChildNum == 2);
		ScanColumn* pExpr = buildScanColumn(pGetTablePlan,
				pChild->m_children[0], pTableInfo, pGetTablePlan->hasGroupBy());

		if (OP_CODE(pChild->m_children[1]) == ASC)
		{
			pGetTablePlan->addSortSpecification(pExpr, true);
		}
		else
		{
			pGetTablePlan->addSortSpecification(pExpr, false);
		}
	}
}

static void buildGroupByPlan(OBScanPlan* pGetTablePlan, ParseNode* pNode)
{
	if (pNode == 0)
		return;

	WorkThreadInfo* pInfo = (WorkThreadInfo*) pthread_getspecific(
			WorkThreadInfo::tls_key);
	assert(pInfo);
	ScanColumnFactory* pFactory = pGetTablePlan->getColumnFactory();
	for (size_t i = 0; i < pNode->m_iChildNum; ++i)
	{
		ParseNode* pChild = pNode->m_children[i];
		assert(pChild);

		if (pChild->m_iType != NAME_NODE)
		{
			LOG(ERROR, "Wrong group by clause!");
			throw new ParseException("Wrong group by clause!");
		}

		if (Tools::isRowKeyNode(pChild))
		{
			LOG(ERROR, "Illegal use of rowkey!");
			throw new ParseException("Illegal use of rowkey!");
		}
		ScanColumn* pColumn = pFactory->getScanColumnInfo(pChild->m_pszValue,
				false);
		if (pColumn == NULL)
		{
			pColumn = pFactory->addScanColumn(pChild->m_pszValue, false);
		}

		pGetTablePlan->addGroupByColumn(pColumn);
	}
}

static void buildProjection(OBScanPlan* pGetTablePlan, ParseNode* pNode,
		TableInfo* pTableInfo)
{
	if (pNode == NULL)
		return;

	assert(pNode && pNode->m_iChildNum > 0);
	if (pNode->m_children[0]->m_iType == INFO_NODE
			&& OP_CODE(pNode->m_children[0]) == ALL_COLUMN)
	{
		for (size_t i = 0; i < pTableInfo->getColumnCount(); ++i)
		{
			DBColumnInfo* pColumnInfo = pTableInfo->getColumn(i);
			struct OBScanPlan::OBProjectionInfo info;
			info.m_type = OBScanPlan::NORMAL;
			const char* pszName = pColumnInfo->m_sName.c_str();
			info.m_pszName = pszName;
			info.m_pColumn = BuildExpressionVisitor::addSimpleScanColumn(
					pszName, pGetTablePlan->getColumnFactory(), pColumnInfo,
					true);

			pGetTablePlan->project(info);
		}
		return;
	}

	for (size_t i = 0; i < pNode->m_iChildNum; ++i)
	{
		ParseNode* pColumn = pNode->m_children[i];
		struct OBScanPlan::OBProjectionInfo info;

		bool bAlias = false;
		if (pColumn->m_iType == OP_NODE && OP_CODE(pColumn) == AS)
		{
			assert(pColumn->m_iChildNum == 2);

			ParseNode* pAlias = pColumn->m_children[1];

			info.m_pszName = pAlias->m_pszValue;

			pColumn = pColumn->m_children[0];

			bAlias = true;
		}
		else
		{
			assert(pColumn->m_pszExpr);

			info.m_pszName = pColumn->m_pszExpr;
		}
		//backward comptible
		if (pColumn->m_iType == FUNC_NODE
				&& strcmp(pColumn->m_pszValue, "to_cash") == 0)
		{
			pColumn = pColumn->m_children[0];
		}

		if (Tools::isRowKeyNode(pColumn))
		{
			info.m_type = OBScanPlan::ROWKEY;
		}
		else if (Tools::isRowCountNode(pColumn))
		{
			info.m_type = OBScanPlan::ROWCOUNT;
		}
		else
		{
			info.m_type = OBScanPlan::NORMAL;

			BuildExpressionVisitor expressionBuilder(
					pGetTablePlan->getColumnFactory(), pTableInfo, pGetTablePlan->hasGroupBy());
			ScanColumn* pExpr = expressionBuilder.walk(pColumn, true);
			assert(pExpr);
			info.m_pColumn = pExpr;
			if (pExpr->getType() == TYPE_UNKNOWN)
			{
			PARSE_ERROR("Unsupported expression %s", pColumn->m_pszExpr);
			}
			if (bAlias)
			{
				pGetTablePlan->getColumnFactory()->alias(info.m_pszName, pExpr);
			}
		}

		pGetTablePlan->project(info);
	}
}

void buildPlanForOBSelect(ParseNode* pNode)
{
	assert(pNode && pNode->m_iChildNum == 7);

	ParseNode* pTable = pNode->m_children[SQL_SELECT_TABLE];
	assert(pTable && pTable->m_iType == NAME_NODE);
	TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pTable->m_pszValue);
	if (pTableInfo == NULL)
	{
		LOG(ERROR, "Undefined table %s", pTable->m_pszValue);
		throw new ParseException("Undefined table %s", pTable->m_pszValue);
	}
	OBScanPlan* pPlan = new OBScanPlan(pTableInfo);
	Tools::pushPlan(pPlan);

	buildOBPredicate(pNode->m_children[SQL_SELECT_PREDICATE]);
	buildGroupByPlan(pPlan, pNode->m_children[SQL_SELECT_GROUPBY]);
	buildProjection(pPlan, pNode->m_children[SQL_SELECT_PROJECT], pTableInfo);
	buildHaving(pPlan, pNode->m_children[SQL_SELECT_HAVING], pTableInfo);
	buildOrderBy(pPlan, pNode->m_children[SQL_SELECT_ORDERBY], pTableInfo);
	buildLimit(pPlan, pNode->m_children[SQL_SELECT_LIMIT]);
}

void buildPlanForOBDelete(ParseNode* pNode)
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

	OBScanPlan* pSearchPlan = new OBScanPlan(pTableInfo);
	
	OBScanPlan::OBProjectionInfo projInfo;
	projInfo.m_type = OBScanPlan::ROWKEY;
	pSearchPlan->project(projInfo);

	assert(pTableInfo->getColumnCount() > 0);
	DBColumnInfo* pColumnInfo = pTableInfo->getColumn(0);
	projInfo.m_type = OBScanPlan::NORMAL;
	const char* pszName = pColumnInfo->m_sName.c_str();
	projInfo.m_pszName = pszName;
	projInfo.m_pColumn = BuildExpressionVisitor::addSimpleScanColumn(
					pszName, pSearchPlan->getColumnFactory(), pColumnInfo,
					true);
	pSearchPlan->project(projInfo);

	Tools::pushPlan(pSearchPlan);

	buildOBPredicate(pNode->m_children[1]);
	Tools::popPlan();

	OBDeletePlan* pPlan = new OBDeletePlan(pTable->m_pszValue, pSearchPlan);
	Tools::pushPlan(pPlan);
#else
	PARSE_ERROR("Delete is not supported");
#endif
}

void buildPlanForOBInsert(ParseNode* pNode)
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

	OBInsertPlan* pPlan = new OBInsertPlan(pTableInfo);
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

