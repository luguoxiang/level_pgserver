#include "common/ConfigInfo.h"
#include "common/MetaConfig.h"
#include "common/ConfigException.h"
#include "common/ParseException.h"
#include <memory>
#include <regex>
#include <glog/logging.h>

void TableInfo::addColumn(MetaConfig* pConfig, const std::string& sValue) {
	std::regex rgx(R"(([^:\s]+):([^:\(\)\s]+)(\([\d]+\))?)");
	std::smatch matches;

	DBColumnInfo* pColumn;

	if (std::regex_search(sValue, matches, rgx)) {
		if (matches.size() < 3) {
			 throw new ConfigException(ConcateToString("Illegal attribute value ", sValue));
		}
		DBColumnInfo* pColumn = new DBColumnInfo(matches.str(1), pConfig->getDataType(matches.str(2)));
		m_columns.emplace_back(pColumn);
		m_columnMap[pColumn->m_name] = pColumn;
		std::string sLen = matches[3];
		if (sLen.length() > 0) {
			pColumn->m_iLen = atoi(sLen.c_str() + 1);
			if (pColumn->m_iLen <= 0) {
				 throw new ConfigException(ConcateToString("Illegal type length ", sLen));
			}
		} else {
			pColumn->m_iLen = GetTypeSize(pColumn->m_type);
		}
	} else {
		 throw new ConfigException(ConcateToString("Illegal attribute value ", sValue));
	}

}

void TableInfo::addKeyColumn(const std::string& name) {
	DBColumnInfo* pColumn = getColumnByName(name);
	if (pColumn == nullptr) {
		 throw new ConfigException(ConcateToString("Undefined rowkey column ", name));
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

void TableInfo::getDBColumns(const ParseNode* pColumn,
		std::vector<DBColumnInfo*>& columns) {
	if (pColumn == 0) {
		//insert into t values(....)
		//add all columns in table
		for (auto& p : m_columns) {
			columns.push_back(p.get());
		}
	} else {
		assert(pColumn->children() > 0);
		for(size_t i=0;i<pColumn->children();++i) {
			auto p = pColumn->getChild(i);
			if (p == nullptr || p->m_type != NodeType::NAME) {
				throw new ParseException(ConcateToString("Unsupported select expression:" , p->m_sExpr));
			}
			DBColumnInfo* pColumnInfo = getColumnByName(p->m_sValue);
			if (pColumnInfo == 0) {
				throw new ParseException(ConcateToString("Table ", m_name, " does not have column named ", p->m_sValue));
			}
			columns.push_back(pColumnInfo);
		}

	}
}
