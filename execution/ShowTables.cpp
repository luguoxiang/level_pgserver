#include "ShowTables.h"
#include "common/MetaConfig.h"
#include <sstream>

std::string ShowTables::getInfoString() {
	return ConcateToString("SELECT ", MetaConfig::getInstance().getTableCount());
}

void ShowTables::begin() {
	m_iIndex = 0;
	MetaConfig::getInstance().getTables(m_tables);
}

bool ShowTables::next() {
	++m_iIndex;
	checkCancellation();
	return m_iIndex <= m_tables.size();
}

void ShowTables::getResult(size_t index, ExecutionResult& result) {
	TableInfo* pTable = m_tables[m_iIndex - 1];
	if (index == 0) {
		result.setStringView(pTable->getName());
	} else if (index == 1) {
		if (pTable->hasAttribute("info")) {
			result.setStringView(pTable->getAttribute("info"));
		} else {
			result.setStringView("");
		}
	} else {
		assert(0);
	}
}
