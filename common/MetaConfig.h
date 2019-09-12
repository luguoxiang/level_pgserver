#pragma once

#include <map>
#include <cassert>
#include "common/ConfigInfo.h"

class MetaConfig {
public:
	~MetaConfig() {}


	void load(const std::string& sPath);

	static MetaConfig& getInstance() {
		static MetaConfig config;
		return config;
	}

	void getTables(std::vector<TableInfo*>& tables) {
		tables.clear();
		for (auto& pv: m_tableMap) {
			tables.push_back(pv.second.get());
		}
	}

	DBDataType getDataType(const std::string_view& sType) const {
		auto iter = m_dataTypeMap.find(sType);
		if (iter == m_dataTypeMap.end())
			return DBDataType::UNKNOWN;
		return iter->second;
	}

	std::string_view typeToString(DBDataType targetType) {
		for(auto& [name, type] : m_dataTypeMap) {
			if (targetType == type) {
				return name;
			}
		}
		return "unknown";
	}

	uint32_t getWorkerNum() const {
		return m_iWorkerNum;
	}
	uint32_t getTimeout() const {
		return m_iTimeout;
	}
	uint32_t getNetworkBuffer() const {
		return m_iNetBuffer;
	}
	uint32_t getExecutionBuffer() const {
		return m_iExecBuffer ;
	}

	int getPort() const { return m_iPort;}

	const TableInfo* getTableInfo(std::string_view name) {
		auto iter = m_tableMap.find(name);
		if (iter != m_tableMap.end()) {
			return iter->second.get();
		} else {
			return nullptr;
		}
	}

	size_t getTableCount() {
		return m_tableMap.size();
	}

	void addTable(TableInfo* pTable);

	void addDataType(std::string_view key, DBDataType value) {
		m_dataTypeMap[key] = value;
	}

	MetaConfig(const MetaConfig&) = delete;
	MetaConfig& operator =(const MetaConfig&) = delete;
private:

	MetaConfig() {}

	//string view on TableInfo.m_name
	std::map<std::string_view, std::unique_ptr<TableInfo>> m_tableMap;
	std::map<std::string_view, DBDataType> m_dataTypeMap;

	uint32_t m_iWorkerNum = 20;
	uint32_t m_iTimeout = 60;
	uint32_t m_iNetBuffer = 1024 * 1024;
	uint32_t m_iExecBuffer = 1024 * 1024 * 1024;
	int m_iPort = 5433;

};
