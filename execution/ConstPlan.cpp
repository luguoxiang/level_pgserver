#include "execution/ConstPlan.h"

void ConstPlan::explain(std::vector<std::string>& rows)
{
	char buf[100];
	snprintf(buf, 100, "Const %d rows, %d columns", m_rows.size(), m_columns.size());
	rows.push_back(buf);
}

DBDataType ConstPlan::getResultType(size_t index)
{
	assert(!m_rows.empty());
	ParseNode* pRow = m_rows[0];
	assert(pRow->m_iChildNum == m_columns.size());
	switch (pRow->m_children[index]->m_iType)
	{
	case INT_NODE:
		return TYPE_INT64;
	case STR_NODE:
		return TYPE_STRING;
	case FLOAT_NODE:
		return TYPE_DOUBLE;
	case DATE_NODE:
		return strlen(pRow->m_children[index]->m_pszValue) < 12 ? TYPE_DATE:TYPE_DATETIME;
	default:
		assert(0);
		return TYPE_UNKNOWN;
	}
}

void ConstPlan::getResult(size_t index, ResultInfo* pInfo)
{
	assert(m_iCurrent > 0 && m_iCurrent <= m_rows.size());
	ParseNode* pRow = m_rows[m_iCurrent - 1];
	assert(pRow->m_iChildNum == m_columns.size());
	pInfo->m_bNull = false;
	ParseNode* pValue = pRow->m_children[index];
	switch (pValue->m_iType)
	{
	case INT_NODE:
		pInfo->m_value.m_lResult = pValue->m_iValue;
		break;
	case DATE_NODE:{
		struct timeval time;
		time.tv_sec = (pValue->m_iValue / 1000000);
		time.tv_usec = (pValue->m_iValue % 1000000);
		pInfo->m_value.m_time = time;
		break;
	}
	case FLOAT_NODE:{
		float fValue = strtof(pValue->m_pszValue, NULL);
		pInfo->m_value.m_dResult = fValue;
		break;
	}
	case STR_NODE:
		pInfo->m_value.m_pszResult = pValue->m_pszValue;
		pInfo->m_len = pValue->m_iValue;
		break;
	default:
		throw new ParseException("wrong const value type %d");
	}
}

int ConstPlan::addProjection(ParseNode* pNode)
{
	assert(pNode);
	if(pNode->m_iType != NAME_NODE) return -1;
	for (size_t i = 0; i < m_columns.size(); ++i)
	{
		if (strcmp(m_columns[i].c_str(), pNode->m_pszValue) == 0)
		{
			return i;
		}
	}
	return -1;
}

void ConstPlan::addRow(ParseNode* pRow)
{
	if(m_rows.empty())
	{
		for(size_t i=0;i<pRow->m_iChildNum;++i)
		{
			char buf[100];
			snprintf(buf, 100, "c%d", i + 1);
			m_columns.push_back(buf);
		}
	}
	assert(pRow->m_iChildNum == m_columns.size());
	m_rows.push_back(pRow);
}