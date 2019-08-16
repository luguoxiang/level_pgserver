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
	std::ostringstream os;
	os << "SELECT " << m_pEntry->getColumnCount();
	return os.str();
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

	pInfo->m_bNull = false;
	switch (index) {
	case 0:
		pInfo->m_sResult = pColumn->m_sName;
		break;
	case 1:
		switch (pColumn->m_type) {
		case DBDataType::INT8:
			pInfo->m_sResult = "int8";
			break;
		case DBDataType::INT16:
			pInfo->m_sResult = "int16";
			break;
		case DBDataType::INT32:
			pInfo->m_sResult = "int32";
			break;
		case DBDataType::INT64:
			pInfo->m_sResult = "int64";
			break;
		case DBDataType::STRING:
			pInfo->m_sResult = "varchar";
			break;
		case DBDataType::DATETIME:
			pInfo->m_sResult = "datetime";
			break;
		case DBDataType::DATE:
			pInfo->m_sResult = "date";
			break;
		case DBDataType::DOUBLE:
			pInfo->m_sResult = "double";
			break;
		case DBDataType::BYTES:
			pInfo->m_sResult = "bytes";
			break;
		default:
			pInfo->m_sResult = "unknown";
			break;
		}
		break;
	case 2:
		if (pColumn->m_iKeyIndex < 0) {
			pInfo->m_bNull = true;
		} else {
			pInfo->m_bNull = false;
			pInfo->m_lResult = pColumn->m_iKeyIndex;
		}
		break;
	case 3:
		if (pColumn->m_iLen < 0) {
			pInfo->m_bNull = true;
		} else {
			pInfo->m_bNull = false;
			pInfo->m_lResult = pColumn->m_iLen;
		}
		break;
	default:
		assert(0);
	}
}
