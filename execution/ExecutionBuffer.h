#pragma once
#include <bitset>
#include <tuple>

#include "ExecutionResult.h"
#include "ExecutionException.h"
#include "common/ConfigInfo.h"

class ExecutionBuffer
{
public:
	ExecutionBuffer(size_t size) : m_executionBuffer(size) {};

	using Row = std::byte*;

	Row beginRow() {
		m_nullBits.reset();
		m_iIndex = 0;
		m_pNullBitsLong = alloc<unsigned long>(0);
		return reinterpret_cast<Row>(m_pNullBitsLong);
	}
	void allocForColumn(DBDataType type, const ExecutionResult& result);

	void endRow() {
		*m_pNullBitsLong = m_nullBits.to_ulong();
	}
	void purge() {
		m_iUsed = 0;
	}

	void getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types);
	int compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types);

	static void init();
private:
	template <class T>
	T* alloc(T t) {
		T* data = reinterpret_cast<T*>(m_executionBuffer.data() + m_iUsed);
		m_iUsed += sizeof(T);
		if (m_iUsed > m_executionBuffer.size()) {
			throw new ExecutionException("not enough execution buffer");
		}
		*data = t;
		return data;
	}
	std::byte* get(Row row, size_t index, const std::vector<DBDataType>& types);

	std::string_view allocString(std::string_view t);

	std::vector<std::byte> m_executionBuffer;

	size_t m_iUsed = 0;
	std::bitset<32> m_nullBits;
	unsigned long* m_pNullBitsLong;
	size_t m_iIndex;

	using SetResultFn = void (*) (std::byte* pData, ExecutionResult& result) ;
	using CompareFn = int (*) (std::byte* pData1, std::byte* pData2);
	using SizeFn = size_t (*)(std::byte* pData);
	using AllocFn =  void (*) (ExecutionBuffer& buffer, const ExecutionResult& result);
	using TypeOperationTuple = std::tuple<SetResultFn, CompareFn,SizeFn, AllocFn>;

	template <class IntType>
	static TypeOperationTuple makeIntTuple();

	static std::map<DBDataType, TypeOperationTuple> m_typeOperations;

};
