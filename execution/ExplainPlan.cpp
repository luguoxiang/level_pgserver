#include "ExplainPlan.h"
#include "common/ParseException.h"
#include "ExecutionException.h"
#include "WorkThreadInfo.h"
#include <algorithm>



void ExplainPlan::getResult(size_t index, ExecutionResult& result) {
	assert(m_iCurrentRow <= m_rows.size());
	assert(m_iCurrentRow > 0);
	result.setStringView(m_rows[m_iCurrentRow - 1]);
}

void ExplainPlan::begin() {
	m_rows.clear();
	m_pPlan->explain(m_rows, 0);
	m_iCurrentRow = 0;
}

bool ExplainPlan::next() {
	++m_iCurrentRow;
	return m_iCurrentRow <= m_rows.size();
}

void ExplainPlan::end() {
}

