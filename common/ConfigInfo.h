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

struct DBColumnInfo {
	DBColumnInfo() : DBColumnInfo {DBDataType::UNKNOWN}{
	}

	DBColumnInfo(DBDataType type) :
			m_type(type) {
	}

	std::string m_sName;
	DBDataType m_type;
	int m_iLen = 0;
	int m_iIndex = 0;
	int m_iKeyIndex = -1;
};

class MetaConfig;

class TableInfo {
public:
	~TableInfo() {
		for (auto p : m_columns) {
			delete p;
		}
	}

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
		return m_columns[i];
	}

	DBColumnInfo* getKeyColumn(size_t i) const {
		return m_keys[i];
	}

	DBColumnInfo* getColumnByName(const std::string& name) {
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

	void getDBColumns(ParseNode* pColumn, std::vector<DBColumnInfo*>& columns);
private:
	std::vector<DBColumnInfo*> m_columns;
	std::vector<DBColumnInfo*> m_keys;
	std::map<std::string, DBColumnInfo*> m_columnMap;

	std::map<std::string, std::string> m_attr;

	std::string m_name;
};

