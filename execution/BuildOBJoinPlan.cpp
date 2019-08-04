#include "common/SqlParser.tab.h"
#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "OBScanPlan.h"
#include "execution/ParseTools.h"
#include "execution/ProjectionPlan.h"

void buildPlanForLeftJoin(ParseNode* pNode)
{
	assert(pNode);
	assert(pNode->m_iChildNum == 4);

	WorkThreadInfo* pWorkerInfo = (WorkThreadInfo*) pthread_getspecific(
			WorkThreadInfo::tls_key);
	assert(pWorkerInfo);

	ParseNode* pProject = pNode->m_children[0];
	ParseNode* pSubQuery = pNode->m_children[1];
	ParseNode* pQueryAlias = pNode->m_children[2];
	ParseNode* pJoinList = pNode->m_children[3];

	BUILD_PLAN(pSubQuery);
	assert(pQueryAlias->m_iType == NAME_NODE);
	assert(pProject->m_iChildNum > 0);

	OBScanPlan* pPlan = static_cast<OBScanPlan*>(Tools::popPlan());
	assert(pPlan);

	if (pPlan->getResultColumns() == 0)
	{
		throw new ParseException(
				"All columns subquery in leftjoin is not supported!");
	}

	const char* pszQueryName = pQueryAlias->m_pszValue;

	typedef std::map<const char*, const char*, Tools::StringCompare> IndexColumnMap;
	IndexColumnMap indexColumnMap;

	for (size_t i = 0; i < pJoinList->m_iChildNum; ++i)
	{
		ParseNode* pJoin = pJoinList->m_children[i];

		assert(pJoin->m_iChildNum == 2);
		assert(pJoin->m_children[0]->m_iType == NAME_NODE);
 		
		ParseNode* pUsingList = pJoin->m_children[1];
    assert(pUsingList->m_iType == PARENT_NODE);
    assert(pUsingList->m_iChildNum >= 1);

		std::string s;
		for(size_t j=0;j<pUsingList->m_iChildNum;++j)
		{
			ParseNode* pNode = pUsingList->m_children[j];
			int iIndex = pPlan->addProjection(pNode);
      if (iIndex < 0)
      {
        PARSE_ERROR("column %s is not defined in subquery", pNode->m_pszValue);
      }
			ScanColumn* pColumn = pPlan->getProjection(iIndex).m_pColumn;
      if (pColumn == NULL)
      {
        PARSE_ERROR("wrong join key '%s'", pNode->m_pszValue);
      }

			s.append(pColumn->getName());
			if (j != pUsingList->m_iChildNum - 1)
			{
				s.append(",");
			}
		}
		const char* pszTable = pJoin->m_children[0]->m_pszValue;
		const char* pszColumn = pWorkerInfo->memdup(s.c_str(), s.size() + 1);

		indexColumnMap[pszTable] = pszColumn;
	}

	ProjectionPlan* pLeftJoin = new ProjectionPlan(pPlan);
	Tools::pushPlan(pLeftJoin);

	for (size_t i = 0; i < pProject->m_iChildNum; ++i)
	{
		ParseNode* pTableColumn = pProject->m_children[i];

		const char* pszTable = NULL;
		const char* pszColumn = NULL;
		const char* pszResultName = NULL;

		if (pTableColumn->m_iType == OP_NODE && OP_CODE(pTableColumn) == AS)
		{
			assert(pTableColumn->m_iChildNum == 2);

			ParseNode* pAlias = pTableColumn->m_children[1];

			pszResultName = pAlias->m_pszValue;

			pTableColumn = pTableColumn->m_children[0];
		}
		switch (pTableColumn->m_iType)
		{
		case NAME_NODE:
			pszTable = pszQueryName;
			pszColumn = pTableColumn->m_pszValue;
			if (pszResultName == NULL)
			{
				pszResultName = pszColumn;
			}
			break;
		case OP_NODE:
			if (OP_CODE(pTableColumn) != '.')
			{
				throw new ParseException(
						"expression is not supported in join statement!");
			}
			pszTable = pTableColumn->m_children[0]->m_pszValue;
			pszColumn = pTableColumn->m_children[1]->m_pszValue;
			if (pszResultName == NULL)
			{
				pszResultName = pTableColumn->m_pszExpr;
			}
			break;
		default:
			throw new ParseException(
					"Unsupported projection column(type = %d) in join statement!",
					pTableColumn->m_iType);
		}

		if (strcmp(pszQueryName, pszTable) != 0)
		{
			IndexColumnMap::iterator iter;
			iter = indexColumnMap.find(pszTable);
			if (iter == indexColumnMap.end())
			{
				PARSE_ERROR("Unrecognized table %s.", pszTable);
			}
			TableInfo* pTableInfo = MetaConfig::getInstance().getTableInfo(pszTable);
			if (pTableInfo == NULL)
			{
				PARSE_ERROR("Undefined table %s", pszTable);
			}
			DBColumnInfo* pColumnInfo = pTableInfo->getColumnByName(pszColumn);
			if (pColumnInfo == NULL)
			{
				PARSE_ERROR("Table %s has no column named %s", pszTable, pszColumn);
			}
			ScanColumn* pColumn = pPlan->getColumnFactory()->addJoinColumn(
					iter->second, pszTable, pszColumn);
			pColumn->setType(pColumnInfo->m_type);

			OBScanPlan::OBProjectionInfo info;
			info.m_pszName = pszColumn;
			info.m_pColumn = pColumn;
			pPlan->project(info);
		}
		ParseNode node;
		node.m_iType = NAME_NODE;
		node.m_iChildNum = 0;
		node.m_pszValue = pszColumn;
		node.m_pszExpr = pszColumn;

		bool bOK = pLeftJoin->project(&node, pszResultName);
		if(!bOK)
		{
			PARSE_ERROR("Unrecognized table column %s", pszColumn);
		}

	}

}
