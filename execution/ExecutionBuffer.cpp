#include "ExecutionBuffer.h"
#include <cassert>
#include <cstring>

void ExecutionBuffer::getResult(DataRow& row, size_t index, ExecutionResult& result) {
	row.getResult(index, result);
}

int ExecutionBuffer::compare(DataRow& row1,DataRow& row2, size_t index) {
	return row1.compare(row2, index);
}

DataRow
ExecutionBuffer::copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types) {
	DataRow row(nullptr, types);
	size_t rowSize = row.computeSize(results);

	auto pData = doAlloc(rowSize);


	row.copy(results, pData);

	return DataRow(pData, types, rowSize);
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
