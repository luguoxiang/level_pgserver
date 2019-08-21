#include "ShowColumns.h"
#include "common/MetaConfig.h"
#include "execution/ParseTools.h"
#include <sstream>
#include <array>
namespace {
std::vector<const char*> Columns = { "Field", "Type", "KeyPosition", "Precision" };
}

std::string ShowColumns::getProjectionName(size_t index) {
	assert(index < 4);
	return Columns[index];
}

void ShowColumns::getAllColumns(std::vector<std::string>& columns) {
	for (auto column: Columns) {
		columns.push_back(column);
	}
}

int ShowColumns::addProjection(ParseNode* pNode) {
	assert(pNode);

	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < Columns.size(); ++i) {
		if (Tools::case_equals(Columns[i], pNode->m_sValue)) {
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
	return ConcateToString("SELECT ", m_pEntry->getColumnCount());
}

void ShowColumns::begin() {
	m_iIndex = 0;
}

bool ShowColumns::next() {
	++m_iIndex;
	return m_iIndex <= m_pEntry->getColumnCount();
}

void ShowColumns::getResult(size_t index, ExecutionResult* pInfo) {
	DBColumnInfo* pColumn = m_pEntry->getColumn(m_iIndex - 1);
	assert(pColumn);

	switch (index) {
	case 0:
		pInfo->setStringView(pColumn->m_sName);
		break;
	case 1:
		pInfo->setStringView(MetaConfig::getInstance().typeToString(pColumn->m_type));
		break;
	case 2:
		if (pColumn->m_iKeyIndex < 0) {
			pInfo->setInt(-1);
		} else {
			pInfo->setInt(pColumn->m_iKeyIndex);
		}
		break;
	case 3:
		if (pColumn->m_iLen < 0) {
			pInfo->setInt(-1);
		} else {
			pInfo->setInt(pColumn->m_iLen);
		}
		break;
	default:
		assert(0);
	}
}
