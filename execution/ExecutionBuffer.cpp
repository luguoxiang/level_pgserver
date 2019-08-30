#include "ExecutionBuffer.h"
#include "DataRow.h"
#include <cassert>
#include <cstring>

void ExecutionBuffer::getResult(Row pData, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types) {
	DataRow row(pData, types);
	row.getResult(index, result);
}

int ExecutionBuffer::compare(Row pData1, Row pData2, size_t index, const std::vector<DBDataType>& types) {
	DataRow row1(pData1, types);
	DataRow row2(pData2, types);
	return row1.compare(row2, index);
}

std::string_view
ExecutionBuffer::copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types) {
	DataRow row(nullptr, types);
	size_t rowSize = row.computeSize(results);

	auto pData = doAlloc(rowSize);


	row.copy(results, pData);

	return std::string_view((const char*)pData, rowSize);
}

std::byte* ExecutionBuffer::doAlloc(size_t size) {
	if(size > GlobalMemBlockPool::BLOCK_SIZE) {
		EXECUTION_ERROR("request buffer too large");
	}
	m_iUsed += size;
	if (m_iUsed > m_iTotal) {
		EXECUTION_ERROR("not enough buffer");
	}

	assert(m_iCurrentBlock <= m_bufferBlocks.size());

	m_iBlockUsed += size;
	if (m_iBlockUsed > GlobalMemBlockPool::BLOCK_SIZE) {
		++m_iCurrentBlock;
		m_iBlockUsed = size;
	}

	if(m_iCurrentBlock == m_bufferBlocks.size()) {
		m_bufferBlocks.push_back(GlobalMemBlockPool::getInstance().alloc());
	}
	return m_bufferBlocks[m_iCurrentBlock]->data() + (m_iBlockUsed - size);
}
