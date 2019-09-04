#pragma once
#include <cstddef>
#include <string>
#include <map>
#include <memory>
#include "common/ConfigInfo.h"
#include "ExecutionResult.h"
#include "ExecutionException.h"

class DBDataTypeHandler {
public:
	DBDataTypeHandler(const std::string& name) : m_name(name) {};
	virtual ~DBDataTypeHandler() {}
	virtual size_t getSize(const ExecutionResult& result) = 0;
	virtual size_t getSize(const std::byte* pData) = 0;

	virtual void fromString(const char* pszValue, size_t len, ExecutionResult& result) = 0;
	virtual void fromNode(const ParseNode* pValue,  ExecutionResult& result) = 0;

	virtual void setToMin(ExecutionResult& result) = 0;
	virtual void setToMax(ExecutionResult& result) = 0;

	virtual void read(const std::byte* pData, ExecutionResult& result) = 0;
	virtual void write(std::byte* pData, const ExecutionResult& result) = 0;

	virtual void div(ExecutionResult& result, size_t value) = 0;
	virtual void add(ExecutionResult& result, const ExecutionResult& add) = 0;
	virtual int compare(const ExecutionResult& a, const ExecutionResult& b) = 0;

	static void init();
	static DBDataTypeHandler* getHandler(DBDataType type) {
		if (auto iter = m_typeHandlers.find(type); iter != m_typeHandlers.end() ){
			return iter->second.get();
		}
		return nullptr;
	}
	std::string getName() {
		return m_name;
	}
private:
	static std::map<DBDataType, std::unique_ptr<DBDataTypeHandler>> m_typeHandlers;
	const std::string m_name;
};

