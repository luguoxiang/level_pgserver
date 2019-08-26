#pragma once

#include <vector>
#include <map>

#include "ExecutionResult.h"

class DataRow {
public:
	DataRow(const std::byte* pData, const std::vector<DBDataType>& types, int iSize = -1);

	void getResult(size_t index, ExecutionResult& result) const ;

	int compare(const DataRow& row, size_t index) const ;

	int compare(const DataRow& row) const ;

	void copy(const std::vector<ExecutionResult>& results, std::byte* pData);

	size_t computeSize(const std::vector<ExecutionResult>& results);

	DataRow(const DataRow&) = delete;
	DataRow& operator =(const DataRow&) = delete;
private:
	const std::byte* m_pData;

	const std::vector<DBDataType>& m_types;

	int m_iSize;
};
