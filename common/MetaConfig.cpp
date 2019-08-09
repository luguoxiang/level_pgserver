#include "MetaConfig.h"
#include "common/Log.h"
#include "common/ConfigException.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>

MetaConfig::MetaConfig() :
		m_iWorkerNum(20), m_iTimeout(60), m_iNetBuffer(1024 * 1024), m_iExecBuffer(
				64 * 1024 * 1024) {
	m_dataTypeMap["int8"] = TYPE_INT8;
	m_dataTypeMap["int16"] = TYPE_INT16;
	m_dataTypeMap["int32"] = TYPE_INT32;
	m_dataTypeMap["int64"] = TYPE_INT64;
	m_dataTypeMap["double"] = TYPE_DOUBLE;
	m_dataTypeMap["varchar"] = TYPE_STRING;
	m_dataTypeMap["datetime"] = TYPE_DATETIME;
	m_dataTypeMap["date"] = TYPE_DATE;
}

void MetaConfig::clean() {
	std::map<std::string, TableInfo*>::iterator iter = m_tableMap.begin();
	while (iter != m_tableMap.end()) {
		delete iter->second;
		++iter;
	}
	m_tableMap.clear();
	std::map<std::string, ServerInfo*>::iterator iter2 = m_serverMap.begin();
	while (iter2 != m_serverMap.end()) {
		delete iter2->second;
		++iter2;
	}
	m_serverMap.clear();
}
static void removeTail(char* pszValue) {
	assert(pszValue);
	int len = strlen(pszValue);
	while (len > 0) {
		switch (pszValue[len - 1]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			pszValue[len - 1] = '\0';
			break;
		default:
			return;
		}
	}
}

void MetaConfig::addTable(std::string name, TableInfo* pTable) {
	if (getTableInfo(name) != nullptr) {
		throw new ConfigException("table %s already defined!", name.c_str());
	}
	pTable->setName(name);
	m_tableMap[name] = pTable;
	LOG(INFO, "add table %s", name.c_str());
}

void MetaConfig::addServer(std::string name, ServerInfo* pServer) {
	if (getServerInfo(name) != nullptr) {
		throw new ConfigException("server %s already defined!", name.c_str());
	}

	pServer->setName(name);
	m_serverMap[name] = pServer;
	LOG(INFO, "add server %s", name.c_str());
}

void MetaConfig::parseServer(ServerInfo* pServer, const char* pszKey,
		const char* pszValue) {
	LOG(INFO, "add attribute %s=%s to server %s", pszKey, pszValue,
			pServer->getName());
	pServer->addAttribute(pszKey, pszValue);
}

void MetaConfig::parseTable(TableInfo* pTable, const char* pszKey,
		const char* pszValue) {
	if (strcmp(pszKey, "name") == 0) {
		addTable(pszValue, pTable);
		return;
	}

	if (strcmp(pszKey, "column") == 0) {
		pTable->addColumn(this, pszValue);
	} else if (strcmp(pszKey, "rowkey") == 0) {
		std::string s;
		for (size_t i = 0;; ++i) {
			char c = pszValue[i];
			switch (c) {
			case ',':
			case '\0':
				if (s.length() > 0) {
					pTable->addKeyColumn(s);
				}
				s = "";
				break;
			case ' ':
			case '\t':
				break;
			default:
				s += c;
				break;
			}
			if (c == '\0')
				break;
		}
	} else {
		LOG(INFO, "add attribute %s=%s to table %s", pszKey, pszValue,
				pTable->getName());
		pTable->addAttribute(pszKey, pszValue);
	}
}

void MetaConfig::load(const char* pszPath) {
	if (pszPath == nullptr || pszPath[0] == '\0')
		return;
	FILE* pFile = fopen(pszPath, "r");
	if (pFile == nullptr) {
		throw new ConfigException("wrong st config file %s", pszPath);
	}

	clean();

	char szBuf[1024];
	char* pszLine = szBuf;
	ServerInfo* pCurrentServer = nullptr;
	TableInfo* pCurrentTable = nullptr;
	while (true) {
		size_t len = 1024;
		int iRead = getline(&pszLine, &len, pFile);
		if (iRead < 0)
			break;
		if (szBuf[0] == '#')
			continue;
		szBuf[iRead] = '\0';
		if (szBuf[0] == '[') {
			std::string line = szBuf;
			size_t pos = line.find("-server]");
			if (pos != std::string::npos) {
				pCurrentServer = new ServerInfo();
				pCurrentTable = nullptr;
				line = line.substr(1, pos - 1);
				addServer(line.c_str(), pCurrentServer);
				continue;
			}
			pos = line.find("table]");
			if (pos != std::string::npos) {
				pCurrentTable = new TableInfo();
				pCurrentServer = nullptr;
				continue;
			}
			throw new ConfigException("unknown config section %s", szBuf);
		}

		char* pszKey = nullptr;
		char* pszValue = nullptr;

		bool bValueBegin = false;
		for (int i = 0; i < iRead; ++i) {
			switch (szBuf[i]) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				break;
			case '#':
				szBuf[i] = '\0';
				i = iRead;
				break;
			case '=':
				if (pszKey == nullptr) {
					throw new ConfigException("wrong config line %s", szBuf);
				}
				szBuf[i] = '\0';
				bValueBegin = true;
				break;
			default:
				if (pszKey == nullptr) {
					pszKey = szBuf + i;
				} else if (bValueBegin && pszValue == nullptr) {
					pszValue = szBuf + i;
				}
				break;
			};
		}
		if (pszKey == nullptr)
			continue;
		if (pszValue == nullptr)
			throw new ConfigException("wrong config line %s", szBuf);

		removeTail(pszKey);
		removeTail(pszValue);

		if (pCurrentServer != nullptr) {
			parseServer(pCurrentServer, pszKey, pszValue);
		} else if (pCurrentTable != nullptr) {
			parseTable(pCurrentTable, pszKey, pszValue);
		} else {
			throw new ConfigException("wong config file for %s", szBuf);
		}
	} //while
	fclose(pFile);
}

