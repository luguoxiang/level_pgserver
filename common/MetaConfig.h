#ifndef META_CONFIG_H
#define META_CONFIG_H

#include <map>
#include <assert.h>
#include "common/ConfigInfo.h"


class MetaConfig
{
public:
	~MetaConfig()
	{
		clean();
	}

	void clean();

	void load(const char* pszPath);

	static MetaConfig& getInstance()
	{
		static MetaConfig config;
		return config;
	}

	void getTables(std::vector<TableInfo*>& tables)
	{
		tables.clear();
		std::map<std::string, TableInfo*>::iterator iter = m_tableMap.begin();
		while(iter != m_tableMap.end())
		{
			tables.push_back(iter->second);
			++iter;
		}
	}

	DBDataType getDataType(const char* pszType)
	{
		std::string name(pszType);
		std::map<std::string, DBDataType>::iterator iter = m_dataTypeMap.find(name);
		if (iter == m_dataTypeMap.end())
			return TYPE_UNKNOWN;
		return iter->second;
	}

	uint32_t getWorkerNum()
	{
		return m_iWorkerNum;
	}
	uint32_t getTimeout()
	{
		return m_iTimeout;
	}
	uint32_t getNetworkBuffer()
	{
		return m_iNetBuffer;
	}
	uint32_t getExecutionBuffer()
	{
		return m_iExecBuffer;
	}

	void setWorkerNum(uint32_t num)
	{
		m_iWorkerNum = num;
	}
	void setTimeout(uint32_t timeout)
	{
		m_iTimeout = timeout;
	}
	void setNetworkBuffer(uint32_t size)
	{
		m_iNetBuffer = size;
	}
	void setExecutionBuffer(uint32_t size)
	{
		m_iExecBuffer = size;
	}

	TableInfo* getTableInfo(std::string name)
	{
		std::map<std::string, TableInfo*>::iterator iter;
		iter = m_tableMap.find(name);
		if (iter != m_tableMap.end())
		{
			return iter->second;
		}
		else
		{
			return NULL;
		}
	}
	ServerInfo* getServerInfo(std::string name)
	{
		std::map<std::string, ServerInfo*>::iterator iter;
		iter = m_serverMap.find(name);
		if (iter != m_serverMap.end())
		{
			return iter->second;
		}
		else
		{
			return NULL;
		}
	}

	size_t getTableCount()
	{
		return m_tableMap.size();
	}

	void addTable(std::string name, TableInfo* pTable);

	void addServer(std::string name, ServerInfo* pServer);
private:

	MetaConfig();

	void parseServer(ServerInfo* pServer, const char* pszKey, const char* pszValue);
	void parseTable(TableInfo* pTable, const char* pszKey, const char* pszValue);

	std::map<std::string, TableInfo*> m_tableMap; 
	std::map<std::string, ServerInfo*> m_serverMap; 
	std::map<std::string, DBDataType> m_dataTypeMap;

	uint32_t m_iWorkerNum;
	uint32_t m_iTimeout;
	uint32_t m_iNetBuffer;
	uint32_t m_iExecBuffer;

};
#endif  // META_CONFIG_H
