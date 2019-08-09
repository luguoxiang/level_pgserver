#include "common/ConfigInfo.h"
#include "common/MetaConfig.h"
#include "common/ConfigException.h"
#include "common/ParseException.h"
#include "common/Log.h"
#include <memory>
#include <stdlib.h>

void TableInfo::addColumn(MetaConfig* pConfig, const char* pszValue) {
	const char* pszType = rindex(pszValue, ':');
	if (pszType == NULL) {
		throw new ConfigException("Illegal attribute value '%s'", pszValue);
	}
	++pszType;

	std::string s;
	int iLen = 0;
	const char* pszEnd = rindex(pszValue, '(');
	if (pszEnd != NULL) {
		iLen = atoi(pszEnd + 1);
		s.assign(pszType, pszEnd - pszType);
	} else {
		s = pszType;
	}

	DBDataType type = pConfig->getDataType(s.c_str());
	if (type == TYPE_UNKNOWN) {
		throw new ConfigException("Unknown data type %s", s.c_str());
	}

	DBColumnInfo* pColumn = new DBColumnInfo();
	pColumn->m_sName.assign(pszValue, pszType - pszValue - 1);
	pColumn->m_type = type;
	pColumn->m_iLen = iLen;

	m_columns.push_back(pColumn);
	m_columnMap[pColumn->m_sName] = pColumn;

	if (iLen <= 0) {
		switch (type) {
		case TYPE_INT8:
			pColumn->m_iLen = 1;
			break;
		case TYPE_INT16:
			pColumn->m_iLen = 2;
			break;
		case TYPE_INT32:
			pColumn->m_iLen = 4;
			break;
		case TYPE_INT64:
		case TYPE_DATETIME:
		case TYPE_DATE:
			pColumn->m_iLen = 8;
			break;
		case TYPE_DOUBLE:
			throw new ConfigException("Missing precision for type %s ",
					s.c_str());
		case TYPE_STRING:
			pColumn->m_iLen = -1;
			break;
		default:
			pColumn->m_iLen = 0;
			break;
		}
	}
}

void TableInfo::addKeyColumn(std::string& name) {
	DBColumnInfo* pColumn = getColumnByName(name);
	if (pColumn == NULL) {
		throw new ConfigException("Undefined rowkey column '%s'", name.c_str());
	}
	if (pColumn->m_iLen <= 0) {
		throw new ConfigException("Missing length for rowkey column config!");
	}
	if (pColumn->m_type == TYPE_DOUBLE) {
		throw new ConfigException(
				"rowkey column with double type is not supported");
	}
	pColumn->m_iKeyIndex = m_keys.size();
	m_keys.push_back(pColumn);
}

void TableInfo::getDBColumns(ParseNode* pColumn,
		std::vector<DBColumnInfo*>& columns) {
	if (pColumn == 0) {
		//insert into t values(....)
		//add all columns in table
		for (size_t j = 0; j < getColumnCount(); ++j) {
			columns.push_back(getColumn(j));
		}
	} else {
		assert(pColumn->m_iChildNum > 0);

		for (size_t j = 0; j < pColumn->m_iChildNum; ++j) {
			ParseNode* p = pColumn->m_children[j];
			if (p == NULL || p->m_iType != NAME_NODE) {
				throw new ParseException("Unsupported select expression:",
						p->m_pszExpr);
			}
			DBColumnInfo* pColumnInfo = getColumnByName(p->m_pszValue);
			if (pColumnInfo == 0) {
				throw new ParseException(
						"Table %s does not have column named %s!", getName(),
						p->m_pszValue);
			}
			columns.push_back(pColumnInfo);
		}
	}
}
