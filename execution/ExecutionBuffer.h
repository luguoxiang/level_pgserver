#pragma once
#include <bitset>
#include <memory>
#include <tuple>
#include <vector>

#include "ExecutionResult.h"
#include "ExecutionException.h"
#include "common/ConfigInfo.h"

class ExecutionBuffer
{
public:
	ExecutionBuffer(size_t size) :  m_iTotal(size) {};

	using Row = std::byte*;

	Row copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types);

	void purge() {
		m_iUsed = 0;
		m_iBlockUsed = 0;
		m_iCurrentBlock = 0;
	}

	void getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types);
	int compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types);

	static void init();
private:
	using BufferBlock = std::vector<std::byte>;
	std::vector<std::unique_ptr<BufferBlock>> m_bufferBlocks;

	static constexpr size_t BLOCK_SIZE = 512 * 1024;

	std::byte* get(Row row, size_t index, const std::vector<DBDataType>& types);
	std::byte* doAlloc(size_t size);

	size_t m_iUsed = 0;
	size_t m_iBlockUsed = 0;
	size_t m_iCurrentBlock = 0;
	const size_t m_iTotal;

	using ReadFn = void (*) (const std::byte* pData, ExecutionResult& result) ;
	using WriteFn = void (*) (std::byte* pData, const ExecutionResult& result) ;
	using CompareFn = int (*) (const std::byte* pData1, const std::byte* pData2);
	using Size1Fn = size_t (*)(const std::byte* pData);
	using Size2Fn = size_t (*)(const ExecutionResult& result);
	using TypeOperationTuple = std::tuple<ReadFn, WriteFn, CompareFn,Size1Fn, Size2Fn>;

	template <class Type>
	static TypeOperationTuple makeTuple();

	static std::map<DBDataType, TypeOperationTuple> m_typeOperations;

};
