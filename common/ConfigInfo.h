#pragma once
#include <map>
#include <vector>
#include <string>
#include <netdb.h>
#include "common/ParseNode.h"

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

inline size_t GetTypeSize(DBDataType type) {
	switch (type) {
	case DBDataType::INT8:
		return sizeof(int8_t);
	case DBDataType::INT16:
		return sizeof(int16_t);
	case DBDataType::INT32:
		return sizeof(int32_t);
	case DBDataType::INT64:
	case DBDataType::DATETIME:
	case DBDataType::DATE:
		return sizeof(int64_t);
	case DBDataType::FLOAT:
		return sizeof(float);
	case DBDataType::DOUBLE:
		return sizeof(double);
	default:
		return -1;
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

	const std::string& getName() const {
		return m_name;
	}

	void addColumn(MetaConfig* pConfig, const std::string& sValue);

	void addKeyColumn(const std::string& name);

	size_t getColumnCount() const {
		return m_columns.size();
	}

	DBColumnInfo* getColumn(size_t i) const {
		return m_columns[i].get();
	}

	DBColumnInfo* getKeyColumn(size_t i) const {
		return m_keys[i];
	}

	DBColumnInfo* getColumnByName(const std::string_view name) {
		auto iter = m_columnMap.find(name);
		if (iter == m_columnMap.end())
			return nullptr;
		return iter->second;
	}

	size_t getKeyCount() {
		return m_keys.size();
	}

	void addAttribute(const std::string& key, const std::string& value) {
		m_attr[key] = value;
	}

	const std::string& getAttribute(const std::string& key) {
		return m_attr[key];
	}

	bool hasAttribute(std::string key) const {
		auto iter = m_attr.find(key);
		return iter != m_attr.end();
	}

	void getDBColumns(const ParseNode* pColumn, std::vector<DBColumnInfo*>& columns);
private:
	std::vector<std::unique_ptr<DBColumnInfo>> m_columns;
	std::vector<DBColumnInfo*> m_keys;

	//string view on column.m_name
	std::map<std::string_view, DBColumnInfo*> m_columnMap;

	std::map<std::string, std::string> m_attr;

	std::string m_name;
};

