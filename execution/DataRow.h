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

	static void init();
	size_t computeSize(const std::vector<ExecutionResult>& results);

	DataRow(const DataRow&) = delete;
	DataRow& operator =(const DataRow&) = delete;
private:
	const std::byte* m_pData;

	const std::vector<DBDataType>& m_types;

	const std::byte* get(size_t index) const ;

	using ReadFn = void (*) (const std::byte* pData, ExecutionResult& result) ;
	using WriteFn = void (*) (std::byte* pData, const ExecutionResult& result) ;
	using CompareFn = int (*) (const std::byte* pData1, const std::byte* pData2);
	using Size1Fn = size_t (*)(const std::byte* pData);
	using Size2Fn = size_t (*)(const ExecutionResult& result);
	using TypeOperationTuple = std::tuple<ReadFn, WriteFn, CompareFn,Size1Fn, Size2Fn>;

	template <class Type>
	static TypeOperationTuple makeTuple();
	int m_iSize;

	static std::map<DBDataType, TypeOperationTuple> m_typeOperations;
};
