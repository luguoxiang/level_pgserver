#include <sstream>
#include <vector>
#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>

#include "ShowColumns.h"
#include "ExecutionResult.h"
#include "common/MetaConfig.h"

namespace {
std::vector<const char*> Columns = { "Field", "Type", "KeyPosition", "Precision" };
}

std::string_view ShowColumns::getProjectionName(size_t index) {
	assert(index < 4);
	return Columns[index];
}

void ShowColumns::getAllColumns(std::vector<std::string_view>& columns) {
	for (auto column: Columns) {
		columns.push_back(column);
	}
}

int ShowColumns::addProjection(const ParseNode* pNode) {
	assert(pNode);

	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < Columns.size(); ++i) {
		if (absl::EqualsIgnoreCase(Columns[i], pNode->getString())) {
			return i;
		}
	}
	return -1;
}

DBDataType ShowColumns::getResultType(size_t index) {
	switch (index) {
	case 0:
	case 1:
		return DBDataType::STRING;
	default:
		return DBDataType::INT64;
	}
}

int ShowColumns::getResultColumns() {
	return 4;
}

std::string ShowColumns::getInfoString() {
	return absl::StrCat("SELECT ", m_pEntry->getColumnCount());
}

void ShowColumns::begin() {
	m_iIndex = 0;
}

bool ShowColumns::next() {
	++m_iIndex;
	return m_iIndex <= m_pEntry->getColumnCount();
}

void ShowColumns::getResult(size_t index, ExecutionResult& result, DBDataType type) {
	const DBColumnInfo* pColumn = m_pEntry->getColumn(m_iIndex - 1);
	assert(pColumn);

	switch (index) {
	case 0:
		result.setStringView(pColumn->m_name);
		break;
	case 1:
		result.setStringView(MetaConfig::getInstance().typeToString(pColumn->m_type));
		break;
	case 2:
		if (pColumn->m_iKeyIndex < 0) {
			result.setInt(-1);
		} else {
			result.setInt(pColumn->m_iKeyIndex);
		}
		break;
	case 3:
		if (pColumn->m_iLen == 0) {
			result.setInt(-1);
		} else {
			result.setInt(pColumn->m_iLen);
		}
		break;
	default:
		assert(0);
	}
}
