#pragma once
#include <map>
#include <vector>
#include <string>
#include <netdb.h>
#include "common/ParseNode.h"
#include "common/ConfigException.h"
enum class DBDataType {
	UNKNOWN = 0,
	INT8,
	INT16,
	INT32,
	INT64,
	STRING,
	DATETIME,
	DATE,
	FLOAT,
	DOUBLE,
	BYTES,
};

inline std::string_view GetTypeName(DBDataType type) {
	switch (type) {
	case DBDataType::INT8:
		return "int8";
	case DBDataType::INT16:
		return "int16";
	case DBDataType::INT32:
		return "int32";
	case DBDataType::INT64:
		return "int64";
	case DBDataType::DATETIME:
		return "datetime";
	case DBDataType::DATE:
		return "date";
	case DBDataType::FLOAT:
		return "float";
	case DBDataType::DOUBLE:
		return "double";
	case DBDataType::STRING:
		return "string";
	default:
		return "unknown";
	}
}

struct DBColumnInfo {
	DBColumnInfo(const std::string name, DBDataType type)
		: m_name(name), m_type(type){
	}

	std::string m_name;
	DBDataType m_type;
	int m_iLen = 0;
	int m_iIndex = 0;
	int m_iKeyIndex = -1;
};

class MetaConfig;

class TableInfo {
public:
	void setName(const std::string& name) {
		m_name = name;
	}

	std::string_view getName() const {
		return m_name;
	}

	void addColumn(const MetaConfig* pConfig, const std::string& sValue);

	void addKeyColumn(const std::string& name);

	size_t getColumnCount() const {
		return m_columns.size();
	}

	const DBColumnInfo* getColumn(size_t i) const {
		return m_columns[i].get();
	}

	const DBColumnInfo* getKeyColumn(size_t i) const {
		return m_keys[i];
	}

	const DBColumnInfo* getColumnByName(const std::string_view name) const {
		auto iter = m_columnMap.find(name);
		if (iter == m_columnMap.end())
			return nullptr;
		return iter->second;
	}

	size_t getKeyCount() const {
		return m_keys.size();
	}

	void addAttribute(const std::string& key, const std::string& value) {
		m_attr[key] = value;
	}

	std::string_view getAttribute(const std::string& key) const {
		auto iter = m_attr.find(key);
		if (iter == m_attr.end()) {
			CONFIG_ERROR("attribute ", key, " for ", m_name, " not found");
		}
		return iter->second;
	}

	std::string getAttribute(const std::string& key, const std::string& defValue) const {
		auto iter = m_attr.find(key);
		if (iter == m_attr.end()) {
			return defValue;
		}
		return iter->second;
	}

	bool hasAttribute(std::string key) const {
		auto iter = m_attr.find(key);
		return iter != m_attr.end();
	}

	void getDBColumns(const ParseNode* pColumn, std::vector<const DBColumnInfo*>& columns) const;
private:
	std::vector<std::unique_ptr<DBColumnInfo>> m_columns;
	std::vector<DBColumnInfo*> m_keys;

	//string view on column.m_name
	std::map<std::string_view, DBColumnInfo*> m_columnMap;

	std::map<std::string, std::string> m_attr;

	std::string m_name;
};

