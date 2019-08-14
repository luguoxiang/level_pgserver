#include "ShowTables.h"
#include "common/MetaConfig.h"
#include <sstream>

std::string ShowTables::getInfoString() {
	std::ostringstream os;
	os << "SELECT " << MetaConfig::getInstance().getTableCount();
	return os.str();
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
		pInfo->m_sResult = pTable->getName();
	} else if (index == 1) {
		if (pTable->hasAttribute("info")) {
			pInfo->m_sResult = pTable->getAttribute("info");
		} else {
			pInfo->m_bNull = true;
		}
	} else {
		assert(0);
	}
}
