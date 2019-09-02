#include "DataRow.h"
#include "ExecutionException.h"
#include "DBDataTypeHandler.h"


void DataRow::getResult(std::vector<ExecutionResult>& results) const  {
	results.resize(m_types.size());

	const std::byte* pStart = m_pData;
	for (size_t i = 0; i < m_types.size(); ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		assert(pHandler);
		pHandler->read(pStart, results[i]);
		pStart += pHandler->getSize(pStart);
	}
	assert(m_iSize == 0 || pStart - m_pData == m_iSize);

}
void DataRow::getResult(size_t index, ExecutionResult& result) const {
	const std::byte* pStart = m_pData;
	assert(index < m_types.size());
	for (size_t i = 0; i <= index; ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		assert(pHandler);
		if(i == index) {
			pHandler->read(pStart, result);
			break;
		} else {
			pStart += pHandler->getSize(pStart);
			assert(m_iSize == 0 || pStart - m_pData < m_iSize);
		}
	}

}

int DataRow::compare(const DataRow& row) const {
	const std::byte* pStartA = m_pData;
	const std::byte* pStartB = row.m_pData;

	for (size_t i = 0; i < m_types.size(); ++i) {
		ExecutionResult a, b;
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		assert(pHandler);
		pHandler->read(pStartA, a);
		pHandler->read(pStartB, b);
		int ret = pHandler->compare(a, b);
		if (ret != 0) {
			return ret;
		}
		pStartA += pHandler->getSize(pStartA);
		pStartB += pHandler->getSize(pStartB);
	}
	return 0;
}

int DataRow::compare(const DataRow& row, size_t index) const {
	ExecutionResult a, b;

	getResult(index, a);
	row.getResult(index, b);
	auto pHandler = DBDataTypeHandler::getHandler(m_types[index]);
	assert(pHandler);
	return pHandler->compare(a, b);
}

size_t DataRow::computeSize(const std::vector<ExecutionResult>& results) {
	size_t rowSize = 0;

	assert(results.size() == m_types.size());
	for (size_t i = 0; i < results.size(); ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		assert(pHandler);
		rowSize += pHandler->getSize(results[i]);
	}
	return rowSize;
}

void DataRow::copy(const std::vector<ExecutionResult>& results,
		std::byte* pData) {
	assert(results.size() == m_types.size());
	for (size_t i = 0; i < results.size(); ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		assert(pHandler);
		pHandler->write(pData, results[i]);
		pData += pHandler->getSize(results[i]);
	}
	assert(m_iSize == 0 || pData - m_pData == m_iSize);
}

