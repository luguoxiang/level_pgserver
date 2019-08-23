#pragma once
#include <memory>
#include <tuple>
#include <vector>
#include <cstddef>
#include <string>
#include <algorithm>

#include "ExecutionResult.h"
#include "ExecutionException.h"
#include "common/ConfigInfo.h"
#include "GlobalMemBlockPool.h"
class ExecutionBuffer
{
public:
	ExecutionBuffer(size_t size) :  m_iTotal(size) {};
	~ExecutionBuffer() {
		GlobalMemBlockPool::getInstance().free(m_bufferBlocks.begin(), m_bufferBlocks.end());
	}

	using Row = std::byte*;

	std::pair<Row, size_t> copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types);

	void purge() {
		m_iUsed = 0;
		m_iBlockUsed = 0;
		m_iCurrentBlock = 0;
	}

	void getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types);
	int compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types);

	std::byte* doAlloc(size_t size);

	char* allocString(const char* src, size_t len) {
		auto target = reinterpret_cast<char*>(doAlloc(len));
		std::copy(src, src+len, target);
		return target;
	}
	ExecutionBuffer(const ExecutionBuffer&) = delete;
	ExecutionBuffer& operator =(const ExecutionBuffer&) = delete;
private:
	std::vector<GlobalMemBlockPool::MemBlockPtr> m_bufferBlocks;

	size_t m_iUsed = 0;
	size_t m_iBlockUsed = 0;
	size_t m_iCurrentBlock = 0;
	const size_t m_iTotal;


};
