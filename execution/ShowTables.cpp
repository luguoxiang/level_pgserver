#include "ShowTables.h"
#include "ExecutionResult.h"
#include "common/MetaConfig.h"

#include <sstream>
#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>

std::string ShowTables::getInfoString() {
	return absl::StrCat("SELECT ", MetaConfig::getInstance().getTableCount());
}

void ShowTables::begin(const std::atomic_bool& bTerminated) {
	m_iIndex = 0;
	MetaConfig::getInstance().getTables(m_tables);
}

bool ShowTables::next(const std::atomic_bool& bTerminated) {
	CheckCancellation(bTerminated);
	++m_iIndex;
	return m_iIndex <= m_tables.size();
}

void ShowTables::getResult(size_t index, ExecutionResult& result, DBDataType type) {
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

int ShowTables::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	if (absl::EqualsIgnoreCase(pNode->getString(), "TableName") )
		return 0;
	if (absl::EqualsIgnoreCase(pNode->getString(), "Info") )
		return 1;
	return -1;
}
