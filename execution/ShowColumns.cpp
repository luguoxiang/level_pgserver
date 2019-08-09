#include "ShowColumns.h"
#include "common/MetaConfig.h"

namespace {
const char* Columns[] = { "Field", "Type", "KeyPosition", "Precision" };
}

const char* ShowColumns::getProjectionName(size_t index) {
	assert(index < 4);
	return Columns[index];
}

void ShowColumns::getAllColumns(std::vector<const char*>& columns) {
	size_t count = sizeof(Columns) / sizeof(const char*);
	for (int i = 0; i < count; ++i) {
		columns.push_back(Columns[i]);
	}
}

int ShowColumns::addProjection(ParseNode* pNode) {
	assert(pNode);
	size_t count = sizeof(Columns) / sizeof(const char*);
	if (pNode->m_iType != NAME_NODE)
		return -1;
	for (size_t i = 0; i < count; ++i) {
		if (strcasecmp(Columns[i], pNode->m_pszValue) == 0) {
			return i;
		}
	}
	return -1;
}

DBDataType ShowColumns::getResultType(size_t index) {
	switch (index) {
	case 0:
	case 1:
		return TYPE_STRING;
	default:
		return TYPE_INT64;
	}
}

int ShowColumns::getResultColumns() {
	return 4;
}

void ShowColumns::getInfoString(char* szBuf, int len) {
	snprintf(szBuf, len, "SELECT %lu", m_pEntry->getColumnCount());
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
		pInfo->m_value.m_pszResult = pColumn->m_sName.c_str();
		pInfo->m_len = pColumn->m_sName.size();
		break;
	case 1:
		switch (pColumn->m_type) {
		case TYPE_INT8:
			pInfo->m_value.m_pszResult = "int8";
			break;
		case TYPE_INT16:
			pInfo->m_value.m_pszResult = "int16";
			break;
		case TYPE_INT32:
			pInfo->m_value.m_pszResult = "int32";
			break;
		case TYPE_INT64:
			pInfo->m_value.m_pszResult = "int64";
			break;
		case TYPE_STRING:
			pInfo->m_value.m_pszResult = "varchar";
			break;
		case TYPE_DATETIME:
			pInfo->m_value.m_pszResult = "datetime";
			break;
		case TYPE_DATE:
			pInfo->m_value.m_pszResult = "date";
			break;
		case TYPE_DOUBLE:
			pInfo->m_value.m_pszResult = "double";
			break;
		case TYPE_BYTES:
			pInfo->m_value.m_pszResult = "bytes";
			break;
		default:
			pInfo->m_value.m_pszResult = "unknown";
			break;
		}
		pInfo->m_len = strlen(pInfo->m_value.m_pszResult);
		break;
	case 2:
		if (pColumn->m_iKeyIndex < 0) {
			pInfo->m_bNull = true;
		} else {
			pInfo->m_bNull = false;
			pInfo->m_value.m_lResult = pColumn->m_iKeyIndex;
		}
		break;
	case 3:
		if (pColumn->m_iLen < 0) {
			pInfo->m_bNull = true;
		} else {
			pInfo->m_bNull = false;
			pInfo->m_value.m_lResult = pColumn->m_iLen;
		}
		break;
	default:
		assert(0);
	}
}
