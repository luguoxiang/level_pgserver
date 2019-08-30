#pragma once

#include <cstddef>
#include <vector>
#include <map>

#include "ExecutionResult.h"

class DataRow {
public:
	DataRow(const std::byte* pData, const std::vector<DBDataType>& types, size_t iSize);

	DataRow(const std::vector<DBDataType>& types):DataRow{nullptr,types, 0} {};

	void getResult(size_t index, ExecutionResult& result) const ;

	void getResult(std::vector<ExecutionResult>& results) const ;

	int compare(const DataRow& row, size_t index) const ;

	int compare(const DataRow& row) const ;

	void copy(const std::vector<ExecutionResult>& results, std::byte* pData);

	size_t computeSize(const std::vector<ExecutionResult>& results);

	const char* data()const {return (const char*)m_pData;}

	size_t size()const {return m_iSize;}

	DataRow& operator =(const DataRow& row) {
		assert(m_types == row.m_types);
		m_pData = row.m_pData;
		m_iSize = row.m_iSize;
		return *this;
	}
private:
	const std::byte* m_pData;

	const std::vector<DBDataType>& m_types;

	size_t m_iSize;
};
