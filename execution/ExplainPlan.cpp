#include "ExplainPlan.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include "common/SqlParser.tab.h"
#include "common/Log.h"
#include "WorkThreadInfo.h"
#include <algorithm>

void ExplainPlan::getInfoString(char* szBuf, int len) {
	snprintf(szBuf, len, "SELECT %lu", m_rows.size());
}

void ExplainPlan::getResult(size_t index, ResultInfo* pInfo) {
	assert(m_iCurrentRow <= m_rows.size());
	assert(m_iCurrentRow > 0);
	pInfo->m_bNull = false;
	std::string& s = m_rows[m_iCurrentRow - 1];
	pInfo->m_value.m_pszResult = s.c_str();
	pInfo->m_len = s.size();
}

void ExplainPlan::begin() {
	m_rows.clear();
	m_pPlan->explain(m_rows);
	m_iCurrentRow = 0;
}

bool ExplainPlan::next() {
	++m_iCurrentRow;
	return m_iCurrentRow <= m_rows.size();
}

void ExplainPlan::end() {
}

