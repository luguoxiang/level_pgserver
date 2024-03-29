#include "ConfigInfo.h"
#include "ConfigException.h"
#include "ParseException.h"
#include "MetaConfig.h"
#include "ParseNode.h"
#include <memory>
#include <regex>
#include <glog/logging.h>

void TableInfo::addColumn(const MetaConfig* pConfig, const std::string& sValue) {
	std::regex rgx(R"(([^:\s]+):([^:\(\)\s]+)(\([\d]+\))?)");
	std::smatch matches;

	DBColumnInfo* pColumn;

	if (std::regex_search(sValue, matches, rgx)) {
		if (matches.size() < 3) {
			 CONFIG_ERROR("Illegal attribute value ", sValue);
		}

		auto type = pConfig->getDataType(matches.str(2));
		if (type == DBDataType::UNKNOWN) {
			CONFIG_ERROR("Unknown datatype ", matches.str(2));
		}

		DBColumnInfo* pColumn = new DBColumnInfo(matches.str(1), type);

		pColumn->m_iIndex = m_columns.size();
		m_columns.emplace_back(pColumn);
		m_columnMap[pColumn->m_name] = pColumn;

		std::string sLen = matches[3];
		if (sLen.length() > 0) {
			auto len = atoi(sLen.c_str() + 1);
			if ( len <= 0) {
				CONFIG_ERROR("Illegal type length ", sLen);
			}
			pColumn->m_iLen = len;
		} else {
			pColumn->m_iLen = 0;
		}
		DLOG(INFO)<<"add column " << pColumn->m_name << ", index="<< pColumn->m_iIndex << ", type=" << matches.str(2);
	} else {
		 CONFIG_ERROR("Illegal attribute value ", sValue);
	}

}

void TableInfo::addKeyColumn(const std::string_view name) {
	if(auto iter = m_columnMap.find(name);  iter != m_columnMap.end()) {
		DBColumnInfo* pColumn = iter->second;
		pColumn->m_iKeyIndex = m_keys.size();
		m_keys.push_back(pColumn);
		DLOG(INFO)<<"add key '" << pColumn->m_name << "', index="<< pColumn->m_iKeyIndex;
	} else{
		 CONFIG_ERROR("Undefined key column '", name, "'");
	}


}

void TableInfo::getDBColumns(const ParseNode* pColumn,
		std::vector<const DBColumnInfo*>& columns) const {
	if (pColumn == nullptr) {
		//insert into t values(....)
		//add all columns in table
		for (auto& p : m_columns) {
			columns.push_back(p.get());
		}
	} else {
		assert(pColumn->children() > 0);
		pColumn->forEachChild([this, &columns](size_t index, auto p) {
			assert(p);
			if (p->m_type != NodeType::NAME) {
				PARSE_ERROR("Unsupported select expression:" , p->m_sExpr);
			}
			const DBColumnInfo* pColumnInfo = getColumnByName(p->getString());
			if (pColumnInfo == 0) {
				PARSE_ERROR("Table ", m_name, " does not have column named ", p->getString());
			}
			columns.push_back(pColumnInfo);
		});
	}
}

void TableInfo::evaluate() {
	for(auto& pColumn : m_columns) {
		if(pColumn->m_iKeyIndex >= 0) {
			continue;
		}

		pColumn->m_iValueIndex = m_values.size();
		m_values.push_back(pColumn.get());
	}

	std::string defPath = "";
	auto path = getAttribute("path", defPath);
	if(path.empty()) {
		CONFIG_ERROR("Table ", m_name, " does not define path attribute before keys");
	}
}
