#include "common/ConfigInfo.h"
#include "common/ConfigException.h"
#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include <memory>
#include <regex>
#include <glog/logging.h>

void TableInfo::addColumn(MetaConfig* pConfig, const std::string& sValue) {
	std::regex rgx(R"(([^:\s]+):([^:\(\)\s]+)(\([\d]+\))?)");
	std::smatch matches;

	DBColumnInfo* pColumn;

	if (std::regex_search(sValue, matches, rgx)) {
		if (matches.size() < 3) {
			 CONFIG_ERROR("Illegal attribute value ", sValue);
		}
		DBColumnInfo* pColumn = new DBColumnInfo(matches.str(1), pConfig->getDataType(matches.str(2)));
		m_columns.emplace_back(pColumn);
		m_columnMap[pColumn->m_name] = pColumn;
		std::string sLen = matches[3];
		if (sLen.length() > 0) {
			pColumn->m_iLen = atoi(sLen.c_str() + 1);
			if (pColumn->m_iLen <= 0) {
				CONFIG_ERROR("Illegal type length ", sLen);
			}
		} else {
			pColumn->m_iLen = GetTypeSize(pColumn->m_type);
		}
	} else {
		 CONFIG_ERROR("Illegal attribute value ", sValue);
	}

}

void TableInfo::addKeyColumn(const std::string& name) {
	DBColumnInfo* pColumn = getColumnByName(name);
	if (pColumn == nullptr) {
		 CONFIG_ERROR("Undefined rowkey column ", name);
	}
	if (pColumn->m_iLen <= 0) {
		CONFIG_ERROR("Missing length for rowkey column config!");
	}
	if (pColumn->m_type == DBDataType::DOUBLE) {
		CONFIG_ERROR(
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
				PARSE_ERROR("Unsupported select expression:" , p->m_sExpr);
			}
			DBColumnInfo* pColumnInfo = getColumnByName(p->m_sValue);
			if (pColumnInfo == 0) {
				PARSE_ERROR("Table ", m_name, " does not have column named ", p->m_sValue);
			}
			columns.push_back(pColumnInfo);
		}

	}
}
