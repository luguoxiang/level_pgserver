#include "DataRow.h"
#include "ExecutionException.h"
#include "DBDataTypeHandler.h"

DataRow::DataRow(const std::byte* pData, const std::vector<DBDataType>& types,
		int iSize) :
		m_types(types), m_pData(pData), m_iSize(iSize) {

}

void DataRow::getResult(size_t index, ExecutionResult& result) const {
	const std::byte* pStart = m_pData;
	assert(index < m_types.size());
	for (size_t i = 0; i <= index; ++i) {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		try {
			pHandler->read(pStart, result);
			assert(!result.isNull());
			pStart += pHandler->getSize(result);
		} catch (const std::bad_variant_access& e) {
			assert(0);
		}
	}
	assert(m_iSize < 0 || pStart - m_pData <= m_iSize);
}

int DataRow::compare(const DataRow& row) const {
	for (size_t i = 0; i < m_types.size(); ++i) {
		ExecutionResult a, b;

		getResult(i, a);
		row.getResult(i, b);

		auto pHandler = DBDataTypeHandler::getHandler(m_types[i]);
		try {
			int ret = pHandler->compare(a, b);
			if (ret != 0) {
				return ret;
			}
		} catch (const std::bad_variant_access& e) {
			assert(0);
		}
	}
	return 0;
}

int DataRow::compare(const DataRow& row, size_t index) const {
	ExecutionResult a, b;

	getResult(index, a);
	row.getResult(index, b);
	try {
		auto pHandler = DBDataTypeHandler::getHandler(m_types[index]);
		return pHandler->compare(a, b);
	} catch (const std::bad_variant_access& e) {
		assert(0);
	}
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
		try {
			pHandler->write(pData, results[i]);
			pData += pHandler->getSize(results[i]);
		} catch (const std::bad_variant_access& e) {
			assert(0);
		}
	}
	assert(m_iSize < 0 || pData - m_pData == m_iSize);
}

