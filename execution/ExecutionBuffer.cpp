#include "ExecutionBuffer.h"

std::map<DBDataType, ExecutionBuffer::TypeOperationTuple> ExecutionBuffer::m_typeOperations;

template <class IntType>
ExecutionBuffer::TypeOperationTuple ExecutionBuffer::makeIntTuple(){
	return std::make_tuple(
			SetResultFn{[] (std::byte* pData, ExecutionResult& result) {
					result.setInt(*reinterpret_cast<IntType*>(pData));
			}},
			CompareFn {[]  (std::byte* pData1, std::byte* pData2) ->int {
				return *reinterpret_cast<IntType*>(pData1) - *reinterpret_cast<IntType*>(pData2);
			}},
			SizeFn{[](std::byte* pData) {
				return sizeof(IntType);
			}},
			AllocFn{[] (ExecutionBuffer& buffer, const ExecutionResult& result) {
				buffer.alloc<IntType>(result.getInt());
			}}
	);
}

void ExecutionBuffer::init() {
	m_typeOperations[DBDataType::INT8] = makeIntTuple<int8_t>();
	m_typeOperations[DBDataType::INT16] = makeIntTuple<int16_t>();
	m_typeOperations[DBDataType::INT32] = makeIntTuple<int32_t>();
	m_typeOperations[DBDataType::INT64] = makeIntTuple<int64_t>();

	m_typeOperations[DBDataType::DATE] = m_typeOperations[DBDataType::INT64];
	m_typeOperations[DBDataType::DATETIME] = m_typeOperations[DBDataType::INT64];

	m_typeOperations[DBDataType::DOUBLE] = std::make_tuple(
			SetResultFn{[] (std::byte* pData, ExecutionResult& result) {
				result.setDouble(*reinterpret_cast<double*>(pData));
			}},
			CompareFn {[]  (std::byte* pData1, std::byte* pData2) ->int {
				double a =  *reinterpret_cast<double*>(pData1);
				double b = *reinterpret_cast<double*>(pData2);
				if (a > b) {
					return 1;
				} else if (a <b ){
					return -1;
				} else {
					return 0;
				}
			}},
			SizeFn{[](std::byte* pData) {
				return sizeof(double);
			}},
			AllocFn{[] (ExecutionBuffer& buffer, const ExecutionResult& result) {
				buffer.alloc<double>(result.getDouble());
			}}
	);

	m_typeOperations[DBDataType::STRING] = std::make_tuple(
			SetResultFn{[] (std::byte* pData, ExecutionResult& result) {
				size_t len = *(reinterpret_cast<uint16_t*>(pData));
				pData += sizeof(uint16_t);
				result.setStringView(std::string_view(reinterpret_cast<const char*>(pData), len));
			}},
			CompareFn {[]  (std::byte* pData1, std::byte* pData2) ->int {
				size_t len1 = *(reinterpret_cast<uint16_t*>(pData1));
				size_t len2 = *(reinterpret_cast<uint16_t*>(pData2));
				pData1 += sizeof(uint16_t);
				pData2 += sizeof(uint16_t);
				std::string_view s1(reinterpret_cast<const char*>(pData1), len1);
				std::string_view s2(reinterpret_cast<const char*>(pData2), len2);
				return s1.compare(s2);
			}},
			SizeFn{[](std::byte* pData) {
				return *(reinterpret_cast<uint16_t*>(pData)) + sizeof(uint16_t);;
			}},
			AllocFn{[] (ExecutionBuffer& buffer, const ExecutionResult& result) {
				buffer.allocString(result.getString());
			}}
	);
	m_typeOperations[DBDataType::BYTES] = m_typeOperations[DBDataType::STRING];
}

void ExecutionBuffer::getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types) {
	std::byte* pData = get(row, index, types);
	if (auto iter = m_typeOperations.find(types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<SetResultFn>(iter->second);
		fn(pData, result);
	} else{
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
	if (auto iter = m_typeOperations.find(types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<CompareFn>(iter->second);
		return fn(pData1, pData2);
	} else{
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
		if (auto iter = m_typeOperations.find(types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<SizeFn>(iter->second);
			size_t size = fn(pStart);
			pStart += size;
		} else{
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

	if (auto iter = m_typeOperations.find(type); iter != m_typeOperations.end()) {
		auto fn = std::get<AllocFn>(iter->second);
		fn(*this, result);
	} else{
		assert(0);
	}
}
