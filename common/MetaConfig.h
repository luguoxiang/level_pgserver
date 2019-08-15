#pragma once

#include <map>
#include <cassert>
#include "common/ConfigInfo.h"

class MetaConfig {
public:
	~MetaConfig() {
		clean();
	}

	void clean();

	void load(const std::string& sPath);

	static MetaConfig& getInstance() {
		static MetaConfig config;
		return config;
	}

	void getTables(std::vector<TableInfo*>& tables) {
		tables.clear();
		for (auto& pv: m_tableMap) {
			tables.push_back(pv.second);
		}
	}

	DBDataType getDataType(const std::string& sType) {
		auto iter = m_dataTypeMap.find(sType);
		if (iter == m_dataTypeMap.end())
			return DBDataType::UNKNOWN;
		return iter->second;
	}

	uint32_t getWorkerNum() {
		return m_iWorkerNum;
	}
	uint32_t getTimeout() {
		return m_iTimeout;
	}
	uint32_t getNetworkBuffer() {
		return m_iNetBuffer;
	}
	uint32_t getExecutionBuffer() {
		return m_iExecBuffer;
	}

	void setWorkerNum(uint32_t num) {
		m_iWorkerNum = num;
	}
	void setTimeout(uint32_t timeout) {
		m_iTimeout = timeout;
	}
	void setNetworkBuffer(uint32_t size) {
		m_iNetBuffer = size;
	}
	void setExecutionBuffer(uint32_t size) {
		m_iExecBuffer = size;
	}

	TableInfo* getTableInfo(const std::string& name) {
		auto iter = m_tableMap.find(name);
		if (iter != m_tableMap.end()) {
			return iter->second;
		} else {
			return nullptr;
		}
	}

	size_t getTableCount() {
		return m_tableMap.size();
	}

	void addTable(const std::string& name, TableInfo* pTable);

	MetaConfig(const MetaConfig&) = delete;
	MetaConfig& operator =(const MetaConfig&) = delete;
private:

	MetaConfig();

	std::map<std::string, TableInfo*> m_tableMap;
	std::map<std::string, DBDataType> m_dataTypeMap;

	uint32_t m_iWorkerNum = 20;
	uint32_t m_iTimeout = 60;
	uint32_t m_iNetBuffer = 1024 * 1024;
	uint32_t m_iExecBuffer = 64 * 1024 * 1024;

};
