#include "common/ConfigInfo.h"
#include "common/MetaConfig.h"
#include "common/ConfigException.h"
#include "common/ParseException.h"
#include "common/Log.h"
#include <memory>
#include <regex>

void TableInfo::addColumn(MetaConfig* pConfig, const std::string& sValue) {
	std::regex rgx("([^:\\s]+):([^:\\(\\)\\s]+)(\\([\\d]+\\))?");
	std::smatch matches;

	DBColumnInfo* pColumn;

	if (std::regex_search(sValue, matches, rgx)) {
		if (matches.size() < 3) {
			throw new ConfigException("Illegal attribute value '%s'",
					sValue.c_str());
		}
		DBColumnInfo* pColumn = new DBColumnInfo();
		pColumn->m_sName = matches[1];
		pColumn->m_type = pConfig->getDataType(matches[2]);

		m_columns.push_back(pColumn);
		m_columnMap[pColumn->m_sName] = pColumn;
		std::string sLen = matches[3];
		if (sLen.length() > 0) {
			pColumn->m_iLen = atoi(sLen.c_str() + 1);
			if (pColumn->m_iLen <= 0) {
				throw new ConfigException("Illegal type length '%s'", sLen.c_str());
			}
		} else {
			switch (pColumn->m_type) {
			case DBDataType::INT8:
				pColumn->m_iLen = 1;
				break;
			case DBDataType::INT16:
				pColumn->m_iLen = 2;
				break;
			case DBDataType::INT32:
				pColumn->m_iLen = 4;
				break;
			case DBDataType::INT64:
			case DBDataType::DATETIME:
			case DBDataType::DATE:
				pColumn->m_iLen = 8;
				break;
			case DBDataType::DOUBLE:
				throw new ConfigException("Missing precision for double type");
			case DBDataType::STRING:
				pColumn->m_iLen = -1;
				break;
			default:
				pColumn->m_iLen = 0;
				break;
			}
		}
	} else {
		throw new ConfigException("Illegal attribute value '%s'",
				sValue.c_str());
	}

}

void TableInfo::addKeyColumn(const std::string& name) {
	DBColumnInfo* pColumn = getColumnByName(name);
	if (pColumn == nullptr) {
		throw new ConfigException("Undefined rowkey column '%s'", name.c_str());
	}
	if (pColumn->m_iLen <= 0) {
		throw new ConfigException("Missing length for rowkey column config!");
	}
	if (pColumn->m_type == DBDataType::DOUBLE) {
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
			if (p == nullptr || p->m_iType != NodeType::NAME) {
				throw new ParseException("Unsupported select expression:",
						p->m_pszExpr);
			}
			DBColumnInfo* pColumnInfo = getColumnByName(p->m_pszValue);
			if (pColumnInfo == 0) {
				throw new ParseException(
						"Table %s does not have column named %s!", getName().c_str(),
						p->m_pszValue);
			}
			columns.push_back(pColumnInfo);
		}
	}
}
