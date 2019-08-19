#include <cassert>
#include <limits>

#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "execution/ExecutionException.h"
#include "execution/ParseTools.h"
#include "common/MetaConfig.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;

void ExecutionBuffer::getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types) {
	std::byte* pData = get(row, index, types);
	switch(types[index]) {
		case DBDataType::INT8:
			result.setInt(*reinterpret_cast<int8_t*>(pData));
			break;
		case DBDataType::INT16:
			result.setInt(*reinterpret_cast<int16_t*>(pData));
			break;
		case DBDataType::INT32:
			result.setInt(*reinterpret_cast<int32_t*>(pData));
			break;
		case DBDataType::INT64:
			result.setInt(*reinterpret_cast<int64_t*>(pData));
			break;
		case DBDataType::BYTES:
		case DBDataType::STRING:{
			size_t len = *(reinterpret_cast<uint16_t*>(pData));
			pData += sizeof(uint16_t);
			result.setString(std::string_view(reinterpret_cast<const char*>(pData), len));
			break;
		}
		case DBDataType::DATE:
		case DBDataType::DATETIME:
			break;
		case DBDataType::DOUBLE: {
			result.setDouble(*reinterpret_cast<double*>(pData));
			break;
		}
		default:
			assert(0);
		}
}

std::string_view ExecutionBuffer::allocString(std::string_view t) {
	if(t.length() > std::numeric_limits<uint16_t>::max() ) {
		throw new ExecutionException("too large string value");
	}
	uint16_t* pSize = alloc<uint16_t>(t.length());
	char* pData = reinterpret_cast<char*>(m_executionBuffer.data() + m_iUsed);
	m_iUsed += t.length();
	if (m_iUsed > m_executionBuffer.size()) {
		throw new ExecutionException("not enough execution buffer");
	}
	memcpy(pData, t.data(), t.length());
	return std::string_view(pData, t.length());
}

int ExecutionBuffer::compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types) {
	std::byte* pData1 = get(row1, index, types);
	std::byte* pData2 = get(row2, index, types);
	switch(types[index]) {
	case DBDataType::INT8:
		return *reinterpret_cast<int8_t*>(pData1) - *reinterpret_cast<int8_t*>(pData2);
	case DBDataType::INT16:
		return *reinterpret_cast<int16_t*>(pData1) - *reinterpret_cast<int16_t*>(pData2);
	case DBDataType::INT32:
		return *reinterpret_cast<int32_t*>(pData1) - *reinterpret_cast<int32_t*>(pData2);
	case DBDataType::INT64:
		return *reinterpret_cast<int64_t*>(pData1) - *reinterpret_cast<int64_t*>(pData2);
	case DBDataType::BYTES:
	case DBDataType::STRING:{
		size_t len1 = *(reinterpret_cast<uint16_t*>(pData1));
		size_t len2 = *(reinterpret_cast<uint16_t*>(pData2));
		pData1 += sizeof(uint16_t);
		pData2 += sizeof(uint16_t);
		std::string_view s1(reinterpret_cast<const char*>(pData1), len1);
		std::string_view s2(reinterpret_cast<const char*>(pData2), len2);
		return s1.compare(s2);
	}
	case DBDataType::DATE:
	case DBDataType::DATETIME:
		return 0;
	case DBDataType::DOUBLE: {
		double a =  *reinterpret_cast<double*>(pData1);
		double b = *reinterpret_cast<double*>(pData2);
		if (a > b) {
			return 1;
		} else if (a <b ){
			return -1;
		} else {
			return 0;
		}
	}
	default:
		assert(0);
	}
}

std::byte* ExecutionBuffer::get(Row row, size_t index, const std::vector<DBDataType>& types) {
	std::byte* pStart = row;
	std::bitset<32> nullBits(*reinterpret_cast<unsigned long*>(pStart));
	pStart+=sizeof(unsigned long);

	for(size_t i = 0;i< index ;++i) {
		if(nullBits[i]) {
			continue;
		}
		switch(types[i]) {
		case DBDataType::INT8:
			pStart += sizeof(int8_t);
			break;
		case DBDataType::INT16:
			pStart += sizeof(int16_t);
			break;
		case DBDataType::INT32:
			pStart += sizeof(int32_t);
			break;
		case DBDataType::INT64:
			pStart += sizeof(int64_t);
			break;
		case DBDataType::BYTES:
		case DBDataType::STRING:
			pStart += *(reinterpret_cast<uint16_t*>(pStart)) + sizeof(uint16_t);
			break;
		case DBDataType::DATETIME:
		case DBDataType::DATE:
			pStart += sizeof(struct timeval);
			break;
		case DBDataType::DOUBLE:
			pStart += sizeof(double);
		default:
			assert(0);
		}
	}
	return pStart;
}

void ExecutionBuffer::allocForColumn(DBDataType type, const ExecutionResult& result) {
	if(result.isNull()) {
		m_nullBits.set(m_iIndex++);
		return;
	}
	++m_iIndex;

	switch(type) {
	case DBDataType::INT8:
		alloc<int8_t>(result.getInt());
		break;
	case DBDataType::INT16:
		alloc<int16_t>(result.getInt());
		break;
	case DBDataType::INT32:
		alloc<int32_t>(result.getInt());
		break;
	case DBDataType::INT64:
		alloc<int64_t>(result.getInt());
		break;
	case DBDataType::BYTES:
	case DBDataType::STRING:
		allocString(result.getString());
		break;
	case DBDataType::DATETIME:
	case DBDataType::DATE:
		alloc(result.getTime());
		break;
	case DBDataType::DOUBLE:
		alloc(result.getDouble());
		break;
	default:
		assert(0);
	}
}

WorkThreadInfo::WorkThreadInfo(int fd, int port, int iIndex, size_t executionBufferSize) :
		m_iListenFd(fd), m_port(port), m_iIndex(iIndex), m_executionBuffer(executionBufferSize) {

	m_result = {};
	if (parseInit(&m_result)) {
		throw new ParseException("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
	m_plans.clear();
}

void WorkThreadInfo::parse(const std::string_view sql) {
	parseSql(&m_result, sql);

	if (m_result.m_pResult == 0) {
		throw new ParseException(&m_result);
	}
}

void WorkThreadInfo::print() {
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}

ExecutionPlan* WorkThreadInfo::resolve() {
	assert(m_result.m_pResult);

	BUILD_PLAN(m_result.m_pResult);

	ExecutionPlan* pPlan = popPlan();
	assert(pPlan && m_plans.empty());
	return pPlan;
}


WorkerManager& WorkerManager::getInstance() {
	static WorkerManager manager;
	return manager;
}



