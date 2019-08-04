#include "execution/UnionAllPlan.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include "common/SqlParser.tab.h"
#include "common/Log.h"
#include "ScanColumnImpl.h"
#include "WorkThreadInfo.h"
#include <algorithm>

void UnionAllPlan::getInfoString(char* szBuf, int len)
{
	snprintf(szBuf, len, "SELECT %lu", m_iCurrentRow);
}

void UnionAllPlan::getResult(size_t index, ResultInfo* pInfo)
{
	if (!m_bLeftDone)
		return m_pLeft->getResult(index, pInfo);
	else
		return m_pRight->getResult(index, pInfo);
}

void UnionAllPlan::begin()
{
	m_pLeft->begin();
	m_pRight->begin();
	m_bLeftDone = false;
	m_iCurrentRow = 0;
}

bool UnionAllPlan::next()
{
	if (!m_bLeftDone)
	{
		if (m_pLeft->next())
		{
			++m_iCurrentRow;
			return true;
		}
		else
		{
			m_bLeftDone = true;
		}
	}assert(m_bLeftDone);
	if (m_pRight->next())
	{
		++m_iCurrentRow;
		return true;
	}
	else
	{
		return false;
	}
}

void UnionAllPlan::end()
{
	m_pLeft->end();
	m_pRight->end();
}

