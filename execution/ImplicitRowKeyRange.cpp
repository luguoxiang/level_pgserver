#include "ImplicitRowKeyRange.h"
#include "common/SqlParser.tab.h"
#include "ParseTools.h"
#include "common/ParseException.h"
#include "common/Log.h"

ImplicitRowKeyRange::ImplicitRowKeyRange(TableInfo* pTableInfo) :
		m_pTableInfo(pTableInfo), m_fields(pTableInfo->getKeyCount()) {
	for (size_t i = 0; i < m_fields.size(); ++i) {
		DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(i);

		m_iLen1 += pColumn->m_iLen;
		m_iLen2 += pColumn->m_iLen;

		m_fields[i].m_pStartValue = nullptr;
		m_fields[i].m_pEndValue = nullptr;
		m_fields[i].m_pExpr1 = nullptr;
		m_fields[i].m_pExpr2 = nullptr;
		m_fields[i].m_bStartInclusive = true;
		m_fields[i].m_bEndInclusive = true;
	}
}

ImplicitRowKeyRange::~ImplicitRowKeyRange() {
	if (m_pszRowKey1) {
		delete[] m_pszRowKey1;
	}
	if (m_pszRowKey2) {
		delete[] m_pszRowKey2;
	}
}

void ImplicitRowKeyRange::setRowKeyFieldData(char* pszRowKey, int64_t iLen,
		ParseNode* pNode, const char* pszColumn) {
	int64_t iValue = 1;
	switch (pNode->m_iType) {
	case BINARY_NODE:
	case STR_NODE:
		if (pNode->m_iValue > iLen) {
			throw new ParseException("Rowkey field %s overflow:%d", pszColumn,
					pNode->m_iValue);
		}
		memcpy(pszRowKey, pNode->m_pszValue, pNode->m_iValue);
		memset(pszRowKey + pNode->m_iValue, 0, iLen - pNode->m_iValue);
		return;
	case DATE_NODE:
	case INT_NODE:
		iValue = pNode->m_iValue;
		break;
	default:
		assert(0);
		return;

	};
	//if (8 != (size_t) iLen) {
	//	throw new ParseException(
	//			"Unsupported rowkey field length:expect %d, actual %lu on %s.",
	//			iLen, 8, pszColumn);
	//}
	const char* pszValue = (const char*) &iValue;
	for (size_t i = 0; i < iLen; ++i) {
		pszRowKey[i] = pszValue[iLen - i - 1];
	}
}

bool ImplicitRowKeyRange::isRangeSearch() {
	for (size_t i = 0; i < m_fields.size(); ++i) {
		DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(i);
		RowKeyField& field = m_fields[i];
		++m_iRowkeySearchCount;
		if (field.m_pStartValue == 0 || field.m_pEndValue == 0
				|| field.m_pStartValue != field.m_pEndValue) {
			return true;
		}
	}
	return false;
}

void ImplicitRowKeyRange::done() {
	m_iRowkeySearchCount = 0;

	for (size_t i = 0; i < m_fields.size(); ++i) {
		DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(i);
		RowKeyField& field = m_fields[i];
		++m_iRowkeySearchCount;
		if (field.m_pStartValue == 0 || field.m_pEndValue == 0
				|| field.m_pStartValue != field.m_pEndValue) {
			//we found a range search
			break;
		}
	}

//For example:
//	select * from t where /*+rowkey, a:8, b:8 */ a> 0 and b > 0;
//There will be two RowKeyField -- a and b.
//After processing RowKeyField a, we will reach here.
//we already have range search a > 0, so b > 0 could not be converted to
//rowkey search, add it to setWhere().
	for (size_t i = m_iRowkeySearchCount; i < m_fields.size(); ++i) {
		RowKeyField& field = m_fields[i];
		if (field.m_pExpr1)
			m_conditions.push_back(field.m_pExpr1);
		if (field.m_pExpr2)
			m_conditions.push_back(field.m_pExpr2);

		if (field.m_pStartValue != 0 || field.m_pEndValue != 0) {
			DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(i);
			LOG(WARN, "Could not optimize search on column %s by rowkey index!",
					pColumn->m_sName.c_str());
		}
		field.m_pStartValue = field.m_pEndValue = 0;
	}
	evaluate();
}

void ImplicitRowKeyRange::evaluate() {
	assert(m_iLen1 > 0);
	assert(m_iLen2 > 0);

	char* pszStartRowKey = new char[m_iLen1];
	char* pszEndRowKey = new char[m_iLen2];

	m_pszRowKey1 = pszStartRowKey;
	m_pszRowKey2 = pszEndRowKey;

	m_bStartInc = true;
	m_bEndInc = true;

	assert(m_iRowkeySearchCount > 0);
	int64_t iStart = 0;
	for (size_t i = 0; i < m_iRowkeySearchCount; ++i) {
		DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(i);
		RowKeyField& field = m_fields[i];
		assert(pColumn->m_iLen > 0);

		m_bStartInc &= field.m_bStartInclusive;
		m_bEndInc &= field.m_bEndInclusive;

		if (field.m_pStartValue != 0) {
			setRowKeyFieldData(pszStartRowKey + iStart, pColumn->m_iLen,
					field.m_pStartValue, pColumn->m_sName.c_str());
		} else {
			memset(pszStartRowKey + iStart, 0, pColumn->m_iLen);
		}

		if (field.m_pEndValue != 0) {
			setRowKeyFieldData(pszEndRowKey + iStart, pColumn->m_iLen,
					field.m_pEndValue, pColumn->m_sName.c_str());
		} else {
			memset(pszEndRowKey + iStart, 0xff, pColumn->m_iLen);
		}

		iStart += pColumn->m_iLen;
	}

	if (m_iRowkeySearchCount == m_fields.size()) {
		DBColumnInfo* pColumn = m_pTableInfo->getKeyColumn(
				m_iRowkeySearchCount - 1);
		if (pColumn->m_type == TYPE_STRING || pColumn->m_type == TYPE_BYTES) {
			int min = m_iLen1 - pColumn->m_iLen;
			if (min <= 0)
				min = 1;
			while (m_iLen2 > min && m_pszRowKey2[m_iLen2 - 1] == 0) {
				--m_iLen2;
			}
			while (m_iLen1 > min && m_pszRowKey1[m_iLen1 - 1] == 0) {
				--m_iLen1;
			}
		}

		return;
	}

	RowKeyField& rangeField = m_fields[m_iRowkeySearchCount - 1];

//For where /*+rowkey, a:8, b:8 */ a> 0;
// we should set pszStartRowKey's byte 8-15 to 0xffffffff
//For where /*+rowkey, a:8, b:8 */ a>= 0;
// we should set pszStartRowKey's byte 8-15 to 0x00000000
	memset(pszStartRowKey + iStart, rangeField.m_bStartInclusive ? 0 : 0xff,
			m_iLen1 - iStart);

//For where /*+rowkey, a:8, b:8 */ a<= 0;
// we should set pszEndRowKey's byte 8-15 to 0xffffffff
//For where /*+rowkey, a:8, b:8 */ a< 0;
// we should set pszEndRowKey's byte 8-15 to 0x00000000
	memset(pszEndRowKey + iStart, rangeField.m_bEndInclusive ? 0xff : 0,
			m_iLen2 - iStart);
}

void ImplicitRowKeyRange::setColumnValue(DBColumnInfo* pColumn,
		ParseNode* pValue) {
	assert(pColumn && pColumn->m_iLen > 0 && pColumn->m_iKeyIndex >= 0);
	RowKeyField& field = m_fields[pColumn->m_iKeyIndex];

	switch (pValue->m_iType) {
	case INT_NODE:
	case DATE_NODE:
	case BINARY_NODE:
	case STR_NODE:
		break;
	default:
		return;
	}

	if (field.m_pStartValue != 0 || field.m_pEndValue != 0) {
		throw new ParseException("conflict search condition!");
	}
	field.m_pStartValue = field.m_pEndValue = pValue;
	field.m_bStartInclusive = true;
	field.m_bEndInclusive = true;
	field.m_pExpr1 = nullptr;
}

bool ImplicitRowKeyRange::parseExpression(int iOpCode, ParseNode* pLeft,
		ParseNode* pRight, ParseNode* pExpr) {
	assert(!Tools::isRowKeyNode(pLeft));
	std::string name(pLeft->m_pszValue);
	DBColumnInfo* pColumn = m_pTableInfo->getColumnByName(name);
	if (pColumn == nullptr || pColumn->m_iKeyIndex < 0)
		return false;
	assert(pColumn->m_iLen > 0 && pColumn->m_iKeyIndex >= 0);

	RowKeyField& field = m_fields[pColumn->m_iKeyIndex];

	switch (pRight->m_iType) {
	case INT_NODE:
	case DATE_NODE:
	case BINARY_NODE:
	case STR_NODE:
		break;
	default:
		return false;
	}

	switch (iOpCode) {
	case COMP_LE:
		if (field.m_pEndValue != 0) {
			throw new ParseException("conflict search condition!");
		}
		field.m_bEndInclusive = true;
		field.m_pEndValue = pRight;
		field.m_pExpr2 = pExpr;
		return true;
	case COMP_LT:
		if (field.m_pEndValue != 0) {
			throw new ParseException("conflict search condition!");
		}
		field.m_bEndInclusive = false;
		field.m_pEndValue = pRight;
		field.m_pExpr2 = pExpr;
		return true;
	case COMP_GE:
		if (field.m_pStartValue != 0) {
			throw new ParseException("conflict search condition!");
		}
		field.m_bStartInclusive = true;
		field.m_pStartValue = pRight;
		field.m_pExpr1 = pExpr;
		return true;
	case COMP_GT:
		if (field.m_pStartValue != 0) {
			throw new ParseException("conflict search condition!");
		}
		field.m_bStartInclusive = false;
		field.m_pStartValue = pRight;
		field.m_pExpr1 = pExpr;
		return true;
	case COMP_EQ:
		if (field.m_pStartValue != 0 || field.m_pEndValue != 0) {
			throw new ParseException("conflict search condition!");
		}
		field.m_pStartValue = field.m_pEndValue = pRight;
		field.m_bStartInclusive = true;
		field.m_bEndInclusive = true;
		field.m_pExpr1 = pExpr;
		return true;
	default:
		return false;
	}
}
