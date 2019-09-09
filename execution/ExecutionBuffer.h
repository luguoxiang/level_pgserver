#pragma once
#include <memory>
#include <tuple>
#include <vector>
#include <cstddef>
#include <string>
#include <algorithm>

#include "DataRow.h"
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

	DataRow copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types);

	void getResult(DataRow& row, size_t index, ExecutionResult& result);
	int compare(DataRow& row1, DataRow& row2, size_t index);

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
