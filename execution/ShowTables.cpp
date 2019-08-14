#include "ShowTables.h"
#include "common/MetaConfig.h"

void ShowTables::getInfoString(char* szBuf, int len) {
	snprintf(szBuf, len, "SELECT %lu",
			MetaConfig::getInstance().getTableCount());
}

void ShowTables::begin() {
	m_iIndex = 0;
	MetaConfig::getInstance().getTables(m_tables);
}

bool ShowTables::next() {
	++m_iIndex;
	return m_iIndex <= m_tables.size();
}

void ShowTables::getResult(size_t index, ResultInfo* pInfo) {
	TableInfo* pTable = m_tables[m_iIndex - 1];
	pInfo->m_bNull = false;
	if (index == 0) {
		pInfo->m_value.m_pszResult = pTable->getName().c_str();
		pInfo->m_len = pTable->getName().length();
	} else if (index == 1) {
		if (pTable->hasAttribute("info")) {
			pInfo->m_value.m_pszResult = pTable->getAttribute("info").c_str();
			pInfo->m_len = strlen(pInfo->m_value.m_pszResult);
		} else {
			pInfo->m_bNull = true;
		}
	} else {
		assert(0);
	}
}
