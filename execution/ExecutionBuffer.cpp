#include "ExecutionBuffer.h"
#include <cassert>
#include <cstring>

std::map<DBDataType, ExecutionBuffer::TypeOperationTuple> ExecutionBuffer::m_typeOperations;

template <class Type>
ExecutionBuffer::TypeOperationTuple ExecutionBuffer::makeTuple(){
	return std::make_tuple(
			ReadFn{[] (const std::byte* pData, ExecutionResult& result)  {
				if constexpr (std::is_same<Type, double>::value || std::is_same<Type, float>::value)  {
					result.setDouble(*reinterpret_cast<const Type*>(pData));
				} else {
					result.setInt(*reinterpret_cast<const Type*>(pData));
				}
			}},
			WriteFn{[] (std::byte* pData, const ExecutionResult& result) {
				if constexpr (std::is_same<Type, double>::value || std::is_same<Type, float>::value) {
					*reinterpret_cast<Type*>(pData) = result.getDouble();
				} else {
					*reinterpret_cast<Type*>(pData) = result.getInt();
				}

			}},
			CompareFn {[]  (const std::byte* pData1, const std::byte* pData2) ->int {
				const Type a = *reinterpret_cast<const Type*>(pData1);
				const Type b = *reinterpret_cast<const Type*>(pData2);
				if (a > b) {
					return 1;
				} else if( a < b) {
					return -1;
				} else {
					return 0;
				}
			}},
			Size1Fn{[](const std::byte* pData) {
				return sizeof(Type);
			}},
			Size2Fn{[] (const ExecutionResult& result) {
				return sizeof(Type);
			}}
	);
}
template <>
ExecutionBuffer::TypeOperationTuple ExecutionBuffer::makeTuple<std::string_view>(){
	return std::make_tuple(
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
}

void ExecutionBuffer::init() {
	m_typeOperations[DBDataType::INT8] = makeTuple<int8_t>();
	m_typeOperations[DBDataType::INT16] = makeTuple<int16_t>();
	m_typeOperations[DBDataType::INT32] = makeTuple<int32_t>();
	m_typeOperations[DBDataType::INT64] = makeTuple<int64_t>();

	m_typeOperations[DBDataType::DATE] = m_typeOperations[DBDataType::INT64];
	m_typeOperations[DBDataType::DATETIME] = m_typeOperations[DBDataType::INT64];

	m_typeOperations[DBDataType::FLOAT] = makeTuple<float>();
	m_typeOperations[DBDataType::DOUBLE] = makeTuple<double>();

	m_typeOperations[DBDataType::STRING] =  makeTuple<std::string_view>();
	m_typeOperations[DBDataType::BYTES] = m_typeOperations[DBDataType::STRING];
}

void ExecutionBuffer::getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types) {
	std::byte* pData = get(row, index, types);
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
	if (auto iter = m_typeOperations.find(types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<CompareFn>(iter->second);
		return fn(pData1, pData2);
	} else{
		assert(0);
	}
}

std::byte* ExecutionBuffer::get(Row row, size_t index, const std::vector<DBDataType>& types) {
	std::byte* pStart = row;

	for(size_t i = 0;i< index ;++i) {
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
	size_t rowSize = 0;

	assert(results.size() == types.size());
	for(size_t i=0;i<results.size();++i) {
		if (auto iter = m_typeOperations.find(types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<Size2Fn>(iter->second);
			rowSize += fn(results[i]);
		} else{
			assert(0);
		}
	}
	auto row = doAlloc(rowSize);

	std::byte* pData = row;

	for(size_t i=0;i<results.size();++i) {
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
