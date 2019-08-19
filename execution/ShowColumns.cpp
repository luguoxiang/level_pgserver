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

void ShowColumns::getResult(size_t index, ResultInfo* pInfo) {
	DBColumnInfo* pColumn = m_pEntry->getColumn(m_iIndex - 1);
	assert(pColumn);

	switch (index) {
	case 0:
		pInfo->setString(pColumn->m_sName);
		break;
	case 1:
		switch (pColumn->m_type) {
		case DBDataType::INT8:
			pInfo->setString("int8");
			break;
		case DBDataType::INT16:
			pInfo->setString("int16");
			break;
		case DBDataType::INT32:
			pInfo->setString("int32");
			break;
		case DBDataType::INT64:
			pInfo->setString("int64");
			break;
		case DBDataType::STRING:
			pInfo->setString("varchar");
			break;
		case DBDataType::DATETIME:
			pInfo->setString("datetime");
			break;
		case DBDataType::DATE:
			pInfo->setString("date");
			break;
		case DBDataType::DOUBLE:
			pInfo->setString("double");
			break;
		case DBDataType::BYTES:
			pInfo->setString("bytes");
			break;
		default:
			pInfo->setString("unknown");
			break;
		}
		break;
	case 2:
		if (pColumn->m_iKeyIndex < 0) {
			pInfo->setNull();
		} else {
			pInfo->setInt(pColumn->m_iKeyIndex);
		}
		break;
	case 3:
		if (pColumn->m_iLen < 0) {
			pInfo->setNull();
		} else {
			pInfo->setInt(pColumn->m_iLen);
		}
		break;
	default:
		assert(0);
	}
}
