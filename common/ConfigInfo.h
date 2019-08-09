#pragma once
#include <map>
#include <vector>
#include <string>
#include <netdb.h>
#include "common/ParseNode.h"

enum DBDataType {
	TYPE_UNKNOWN,
	TYPE_INT8,
	TYPE_INT16,
	TYPE_INT32,
	TYPE_INT64,
	TYPE_STRING,
	TYPE_DATETIME,
	TYPE_DATE,
	TYPE_DOUBLE,
	TYPE_BYTES,
};

struct DBColumnInfo {
	DBColumnInfo() :
			m_type(TYPE_UNKNOWN), m_iLen(0), m_iIndex(0), m_iKeyIndex(-1) {
	}

	DBColumnInfo(DBDataType type) :
			m_type(type), m_iLen(0), m_iIndex(0), m_iKeyIndex(-1) {
	}

	std::string m_sName;
	DBDataType m_type;
	int m_iLen;
	int m_iIndex;
	int m_iKeyIndex;

	const char* getName() {
		return m_sName.c_str();
	}
};

class MetaConfig;
class ServerInfo;

class TableInfo {
public:
	~TableInfo() {
		for (size_t i = 0; i < m_columns.size(); ++i) {
			delete m_columns[i];
		}
	}

	void setName(std::string& name) {
		m_name = name;
	}

	const char* getName() const {
		return m_name.c_str();
	}
	;

	void addColumn(MetaConfig* pConfig, const char* pszInfo);

	void addKeyColumn(std::string& name);

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
		std::map<std::string, DBColumnInfo*>::iterator iter;
		iter = m_columnMap.find(name);
		if (iter == m_columnMap.end())
			return nullptr;
		return iter->second;
	}

	size_t getKeyCount() {
		return m_keys.size();
	}

	void addAttribute(std::string key, std::string value) {
		m_attr[key] = value;
	}

	const std::string& getAttribute(std::string key) {
		return m_attr[key];
	}

	bool hasAttribute(std::string key) const {
		std::map<std::string, std::string>::const_iterator iter;
		iter = m_attr.find(key);
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

class ServerInfo {
public:
	void setName(std::string name) {
		m_name = name;
	}

	const char* getName() const {
		return m_name.c_str();
	}
	;

	void addAttribute(std::string key, std::string value) {
		m_attr[key] = value;
	}

	const std::string& getAttribute(std::string key) {
		return m_attr[key];
	}

	bool hasAttribute(std::string key) const {
		std::map<std::string, std::string>::const_iterator iter;
		iter = m_attr.find(key);
		return iter != m_attr.end();
	}

private:
	std::string m_name;
	std::map<std::string, std::string> m_attr;
};

