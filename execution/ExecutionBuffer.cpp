#include "ExecutionBuffer.h"
#include <cassert>
std::map<DBDataType, ExecutionBuffer::TypeOperationTuple> ExecutionBuffer::m_typeOperations;

using ReadFn = void (*) (std::byte* pData, ExecutionResult& result) ;
using WriteFn = std::byte* (*) (std::byte* pData, const ExecutionResult& result) ;
using CompareFn = int (*) (const std::byte* pData1, const std::byte* pData2);
using Size1Fn = size_t (*)(const std::byte* pData);
using Size2Fn = size_t (*)(const ExecutionResult& result);

template <class IntType>
ExecutionBuffer::TypeOperationTuple ExecutionBuffer::makeIntTuple(){
	return std::make_tuple(
			ReadFn{[] (const std::byte* pData, ExecutionResult& result)  {
					result.setInt(*reinterpret_cast<const IntType*>(pData));
			}},
			WriteFn{[] (std::byte* pData, const ExecutionResult& result) {
				*reinterpret_cast<IntType*>(pData) = result.getInt();
			}},
			CompareFn {[]  (const std::byte* pData1, const std::byte* pData2) ->int {
				return *reinterpret_cast<const IntType*>(pData1) - *reinterpret_cast<const IntType*>(pData2);
			}},
			Size1Fn{[](const std::byte* pData) {
				return sizeof(IntType);
			}},
			Size2Fn{[] (const ExecutionResult& result) {
				return sizeof(IntType);
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
			ReadFn{[] (const std::byte* pData, ExecutionResult& result)  {
					result.setDouble(*reinterpret_cast<const double*>(pData));
			}},
			WriteFn{[] (std::byte* pData, const ExecutionResult& result) {
				*reinterpret_cast<double*>(pData) = result.getDouble();
			}},
			CompareFn {[]  (const std::byte* pData1, const std::byte* pData2) ->int {
				double a =  *reinterpret_cast<const double*>(pData1);
				double b = *reinterpret_cast<const double*>(pData2);
				if (a > b) {
					return 1;
				} else if (a <b ){
					return -1;
				} else {
					return 0;
				}
			}},
			Size1Fn{[](const std::byte* pData) {
				return sizeof(double);
			}},
			Size2Fn{[] (const ExecutionResult& result) {
				return sizeof(double);
			}}
	);

	m_typeOperations[DBDataType::STRING] = std::make_tuple(
			ReadFn{[] (const std::byte* pData, ExecutionResult& result) {
				size_t len = *(reinterpret_cast<const uint16_t*>(pData));
				pData += sizeof(uint16_t);
				result.setStringView(std::string_view(reinterpret_cast<const char*>(pData), len));
			}},
			WriteFn{[] (std::byte* pData, const ExecutionResult& result) {
				auto s = result.getString();
				auto size = s.length();
				if(size > std::numeric_limits<uint16_t>::max() ) {
					throw new ExecutionException("too large string value");
				}

				*reinterpret_cast<uint16_t*>(pData) = size;
				pData  += sizeof(uint16_t);
				memcpy(pData, s.data(), size);
			}},
			CompareFn {[]  (const std::byte* pData1,const std::byte* pData2) ->int {
				size_t len1 = *(reinterpret_cast<const uint16_t*>(pData1));
				size_t len2 = *(reinterpret_cast<const uint16_t*>(pData2));
				pData1 += sizeof(uint16_t);
				pData2 += sizeof(uint16_t);
				std::string_view s1(reinterpret_cast<const char*>(pData1), len1);
				std::string_view s2(reinterpret_cast<const char*>(pData2), len2);
				return s1.compare(s2);
			}},
			Size1Fn{[](const std::byte* pData) {
				return *(reinterpret_cast<const uint16_t*>(pData)) + sizeof(uint16_t);
			}},
			Size2Fn{[](const ExecutionResult& result) {
				return result.getString().length() + sizeof(uint16_t);
			}}
	);
	m_typeOperations[DBDataType::BYTES] = m_typeOperations[DBDataType::STRING];
}

void ExecutionBuffer::getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types) {
	std::byte* pData = get(row, index, types);
	if (pData == nullptr) {
		result.setNull();
		return;
	}
	if (auto iter = m_typeOperations.find(types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<ReadFn>(iter->second);
		fn(pData, result);
	} else{
		assert(0);
	}
}

int ExecutionBuffer::compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types) {
	std::byte* pData1 = get(row1, index, types);
	std::byte* pData2 = get(row2, index, types);
	if(pData1 == nullptr) {
		if (pData2 == nullptr) {
			return 0;
		} else {
			return -1;
		}
	} else if (pData2 == nullptr) {
		return 1;
	}
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

	if(nullBits[index]) {
		return nullptr;
	}

	pStart+=sizeof(unsigned long);

	for(size_t i = 0;i< index ;++i) {
		if(nullBits[i]) {
			continue;
		}
		if (auto iter = m_typeOperations.find(types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<Size1Fn>(iter->second);
			size_t size = fn(pStart);
			pStart += size;
		} else{
			assert(0);
		}
	}
	return pStart;
}

ExecutionBuffer::Row ExecutionBuffer::copyRow(const std::vector<ExecutionResult>& results, const std::vector<DBDataType>& types) {
	std::bitset<32> m_nullBits;
	size_t rowSize = sizeof(unsigned long);

	assert(results.size() == types.size());
	for(size_t i=0;i<results.size();++i) {
		if(results[i].isNull()) {
			m_nullBits.set(i);
			continue;
		}
		if (auto iter = m_typeOperations.find(types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<Size2Fn>(iter->second);
			rowSize += fn(results[i]);
		} else{
			assert(0);
		}
	}
	auto row = doAlloc(rowSize);

	auto pNullBitsLong = reinterpret_cast<unsigned long*>(row);
	*pNullBitsLong = m_nullBits.to_ulong();

	std::byte* pData = row + sizeof(unsigned long);

	for(size_t i=0;i<results.size();++i) {
		if(results[i].isNull()) {
			continue;
		}
		if (auto iter = m_typeOperations.find(types[i]); iter != m_typeOperations.end()) {
			auto writeFn = std::get<WriteFn>(iter->second);
			writeFn(pData, results[i]);
			auto sizeFn = std::get<Size1Fn>(iter->second);
			pData +=sizeFn(pData);
		} else{
			assert(0);
		}
	}
	return row;
}

std::byte* ExecutionBuffer::doAlloc(size_t size) {
	if(size > BLOCK_SIZE) {
		throw new ExecutionException("request buffer too large");
	}
	m_iUsed += size;
	if (m_iUsed > m_iTotal) {
		throw new ExecutionException("not enough execution buffer");
	}

	assert(m_iCurrentBlock <= m_bufferBlocks.size());

	m_iBlockUsed += size;
	if (m_iBlockUsed > BLOCK_SIZE) {
		++m_iCurrentBlock;
		m_iBlockUsed = size;
	}

	if(m_iCurrentBlock == m_bufferBlocks.size()) {
		m_bufferBlocks.push_back(std::make_unique<BufferBlock>(BLOCK_SIZE));
	}
	return m_bufferBlocks[m_iCurrentBlock]->data() + (m_iBlockUsed - size);
}
