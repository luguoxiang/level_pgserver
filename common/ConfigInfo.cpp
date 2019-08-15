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
			 std::ostringstream os;
			 os<<"Illegal attribute value " << sValue;
			 throw new ConfigException(os.str());
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
				 std::ostringstream os;
				 os<<"Illegal type length "<< sLen;
				 throw new ConfigException(os.str());
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
		 std::ostringstream os;
		 os<<"Illegal attribute value "<< sValue;
		 throw new ConfigException(os.str());
	}

}

void TableInfo::addKeyColumn(const std::string& name) {
	DBColumnInfo* pColumn = getColumnByName(name);
	if (pColumn == nullptr) {
		 std::ostringstream os;
		 os<<"Undefined rowkey column " << name;
		 throw new ConfigException(os.str());
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
		for (auto p : m_columns) {
			columns.push_back(p);
		}
	} else {
		assert(pColumn->children() > 0);
		for(auto p: pColumn->m_children) {
			if (p == nullptr || p->m_type != NodeType::NAME) {
				std::ostringstream os;
				os<<"Unsupported select expression:" << p->m_sExpr;
				throw new ParseException(os.str());
			}
			DBColumnInfo* pColumnInfo = getColumnByName(p->m_sValue);
			if (pColumnInfo == 0) {
				std::ostringstream os;
				os << "Table "<<m_name<< " does not have column named "<< p->m_sValue;
				throw new ParseException(os.str());
			}
			columns.push_back(pColumnInfo);
		}

	}
}
