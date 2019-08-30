#include "DataRow.h"
#include "ExecutionException.h"
#include "DBDataTypeHandler.h"

DataRow::DataRow(const std::byte* pData, const std::vector<DBDataType>& types,
		size_t iSize) :
		m_types(types), m_pData(pData), m_iSize(iSize) {

}

void DataRow::getResult(size_t index, ExecutionResult& result) const {
	const std::byte* pStart = m_pData;
	assert(index < m_types.size());
	for (size_t i = 0; i <= index; ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
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
	return pHandler->compare(a, b);
}

size_t DataRow::computeSize(const std::vector<ExecutionResult>& results) {
	size_t rowSize = 0;

	assert(results.size() == m_types.size());
	for (size_t i = 0; i < results.size(); ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		rowSize += pHandler->getSize(results[i]);
	}
	return rowSize;
}

void DataRow::copy(const std::vector<ExecutionResult>& results,
		std::byte* pData) {
	assert(results.size() == m_types.size());
	for (size_t i = 0; i < results.size(); ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		pHandler->write(pData, results[i]);
		pData += pHandler->getSize(results[i]);
	}
	assert(m_iSize == 0 || pData - m_pData == m_iSize);
}

