#include "BuildExpressionVisitor.h"
#include "WorkThreadInfo.h"
#include "common/Log.h"

ScanColumn* BuildExpressionVisitor::visitDataNode(ParseNode* pNode, bool) {
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	ScanColumn* pColumn = nullptr;
	switch (pNode->m_iType) {
	case NULL_NODE:
		return m_pFactory->addConstValue("null");
	case INT_NODE:
		pColumn = m_pFactory->addConstValue(pNode->m_pszValue);
		pColumn->setType(DBDataType::INT64);
		return pColumn;
	case FLOAT_NODE:
		pColumn = m_pFactory->addConstValue(pNode->m_pszValue);
		pColumn->setType(DBDataType::DOUBLE);
		return pColumn;
	case BINARY_NODE:
		pColumn = m_pFactory->addConstValue(pNode->m_pszValue);
		return pColumn;
	case STR_NODE:
		pColumn = m_pFactory->addConstValue(
				Tools::escapeString(pNode->m_pszValue, pNode->m_iValue));
		pColumn->setType(DBDataType::STRING);
		return pColumn;
	case DATE_NODE: {
		std::string sExpr = "#";
		sExpr.append(pNode->m_pszValue);
		sExpr.append("#");
		pColumn = m_pFactory->addConstValue(
				pInfo->memdup(sExpr.c_str(), sExpr.size() + 1));
		pColumn->setType(DBDataType::DOUBLE);
		return pColumn;
	}
	default:
		PARSE_ERROR("Unsupported data type %s,%d", pNode->m_pszExpr,
				pNode->m_iType)
		;
		return nullptr;
	};
}

ScanColumn* BuildExpressionVisitor::addSimpleScanColumn(const char* pszName,
		ScanColumnFactory* pFactory, DBColumnInfo* pColumnInfo, bool bProject) {
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	bool bColumnProject = bProject && pColumnInfo->m_type != DBDataType::DOUBLE;
	ScanColumn* pColumn = pFactory->getScanColumnInfo(pszName, bColumnProject);
	if (pColumn == nullptr) {
		pColumn = pFactory->addScanColumn(pszName, bColumnProject);
	}

	if (pColumnInfo->m_type == DBDataType::DOUBLE) {
		char buf[100];
		assert(pColumnInfo->m_iLen > 0);
		int count = snprintf(buf, 100, "%s / %d.0", pszName,
				pColumnInfo->m_iLen);
		pColumn = pFactory->getScanColumnInfo(buf, bProject);
		if (pColumn == nullptr) {
			const char* pszExpr = pInfo->memdup(buf, count + 1);
			pColumn = pFactory->addScanComplexColumn(pszExpr, pszExpr, bProject,
					true);
		}
	}
	pColumn->setType(pColumnInfo->m_type);
	return pColumn;
}

ScanColumn* BuildExpressionVisitor::visitNameNode(ParseNode* pNode,
		bool bProject) {
	assert(!Tools::isRowKeyNode(pNode));

	ScanColumn* pColumn = m_pFactory->getAliasColumnInfo(pNode->m_pszValue);
	if (pColumn != nullptr) {
		return pColumn;
	}
	std::string name(pNode->m_pszValue);
	DBColumnInfo* pColumnInfo = m_pTableInfo->getColumnByName(name);
	if (pColumnInfo == nullptr) {
		PARSE_ERROR("Table %s has no column named %s", m_pTableInfo->getName(),
				pNode->m_pszValue);
	}

	return addSimpleScanColumn(pNode->m_pszValue, m_pFactory, pColumnInfo,
			bProject);
}

ScanColumn* BuildExpressionVisitor::visitDyadicOpNode(int op, ParseNode* pNode,
		bool bProject) {
	assert(pNode->m_pszExpr);

	ScanColumn* pColumn = m_pFactory->getScanColumnInfo(pNode->m_pszExpr,
			bProject);
	if (pColumn != nullptr)
		return pColumn;

	ScanColumn* pLeftColumn = walk(pNode->m_children[0], false);
	assert(pLeftColumn);

	ScanColumn* pRightColumn = walk(pNode->m_children[1], false);
	assert(pRightColumn);

	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);

	char szBuf[101];
	szBuf[100] = '\0';
	int c = snprintf(szBuf, 100, "%s %s %s", pLeftColumn->getName(),
			getOpString(op), pRightColumn->getName());

	pColumn = m_pFactory->addScanComplexColumn(pInfo->memdup(szBuf, c + 1),
			pNode->m_pszExpr, bProject, !m_bGroupBy);

	if (pLeftColumn->getType() == DBDataType::DOUBLE
			|| pRightColumn->getType() == DBDataType::DOUBLE) {
		pColumn->setType(DBDataType::DOUBLE);
	} else if (pLeftColumn->getType() != DBDataType::UNKNOWN) {
		pColumn->setType(pLeftColumn->getType());
	} else if (pRightColumn->getType() != DBDataType::UNKNOWN) {
		pColumn->setType(pRightColumn->getType());
	}
	return pColumn;
}

ScanColumn* BuildExpressionVisitor::visitUnaryOpNode(int op, ParseNode* pNode,
		bool bProject) {
	assert(pNode->m_pszExpr);

	ScanColumn* pColumn = m_pFactory->getScanColumnInfo(pNode->m_pszExpr,
			bProject);
	if (pColumn != nullptr)
		return pColumn;

	ScanColumn* pChildColumn = walk(pNode->m_children[0], false);
	assert(pChildColumn);

	char szBuf[101];
	szBuf[100] = '\0';
	int c = 0;
	switch (op) {
	case '-':
		c = snprintf(szBuf, 100, "-%s", pChildColumn->getName());
		break;
	default:
		PARSE_ERROR("Unsupported expression operator %d!", op)
		;
		break;
	}

	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	pColumn = m_pFactory->addScanComplexColumn(pInfo->memdup(szBuf, c + 1),
			pNode->m_pszExpr, bProject, !m_bGroupBy);
	pColumn->setType(pChildColumn->getType());
	return pColumn;
}

ScanColumn* BuildExpressionVisitor::visitFuncNode(const char* pszName,
		ParseNode* pNode, bool bProject) {
	assert(pNode->m_pszExpr);

	m_bGroupBy = true;
	ScanColumn* pColumn = m_pFactory->getScanColumnInfo(pNode->m_pszExpr,
			bProject);
	if (pColumn != nullptr)
		return pColumn;

	//function parameter is not supported
	m_bGroupBy = false;
	ScanColumn* pParamExpr = walk(pNode->m_children[0], false);
	m_bGroupBy = true;

	pColumn = m_pFactory->addFunctionColumn(pszName, pNode->m_pszExpr,
			pParamExpr->getName(), bProject);
	if (strcmp(pNode->m_pszValue, "count") == 0) {
		pColumn->setType(DBDataType::INT64);
	} else {
		pColumn->setType(pParamExpr->getType());
	}
	return pColumn;
}
