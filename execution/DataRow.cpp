#include "DataRow.h"
#include "ExecutionException.h"

std::map<DBDataType, DataRow::TypeOperationTuple> DataRow::m_typeOperations;

template <class Type>
DataRow::TypeOperationTuple DataRow::makeTuple(){
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
DataRow::TypeOperationTuple DataRow::makeTuple<std::string_view>(){
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
					EXECUTION_ERROR("too large string value");
				}

				*reinterpret_cast<uint16_t*>(pData) = size;
				pData  += sizeof(uint16_t);
				auto pSrc = reinterpret_cast<const std::byte*>(s.data());
				std::copy(pSrc, pSrc + size, pData);
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

void DataRow::init() {
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

DataRow::DataRow(const std::byte* pData,  const std::vector<DBDataType>& types, int iSize)
	: m_types(types), m_pData(pData), m_iSize(iSize) {

}

const std::byte* DataRow::get(size_t index) const {
	const std::byte* pStart = m_pData;
	assert(index < m_types.size());
	for(size_t i = 0;i< index ;++i) {
		if (auto iter = m_typeOperations.find(m_types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<Size1Fn>(iter->second);
			size_t size = fn(pStart);
			pStart += size;
		} else{
			assert(0);
		}
	}
	assert(m_iSize < 0 || pStart - m_pData < m_iSize);
	return pStart;
}

void DataRow::getResult(size_t index, ExecutionResult& result)const  {
	assert(index < m_types.size());
	const std::byte* pData = get(index);
	if (auto iter = m_typeOperations.find(m_types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<ReadFn>(iter->second);
		fn(pData, result);
	} else{
		assert(0);
	}
}

int DataRow::compare(const DataRow& row) const {
	for(size_t index = 0; index < m_types.size(); ++index) {
		const std::byte* pData1 = get(index);
		const std::byte* pData2 = row.get(index);
		if (auto iter = m_typeOperations.find(m_types[index]); iter != m_typeOperations.end()) {
			auto fn = std::get<CompareFn>(iter->second);
			int ret = fn(pData1, pData2);
			if (ret != 0 ){
				return ret;
			}
		} else{
			assert(0);
		}
	}
	return 0;
}

int DataRow::compare(const DataRow& row, size_t index) const {
	const std::byte* pData1 = get(index);
	const std::byte* pData2 = row.get(index);
	if (auto iter = m_typeOperations.find(m_types[index]); iter != m_typeOperations.end()) {
		auto fn = std::get<CompareFn>(iter->second);
		return fn(pData1, pData2);
	} else{
		assert(0);
	}
}

size_t DataRow::computeSize(const std::vector<ExecutionResult>& results) {
	size_t rowSize = 0;

	assert(results.size() == m_types.size());
	for(size_t i=0;i<results.size();++i) {
		if (auto iter = m_typeOperations.find(m_types[i]); iter != m_typeOperations.end()) {
			auto fn = std::get<Size2Fn>(iter->second);
			rowSize += fn(results[i]);
		} else{
			assert(0);
		}
	}
	return rowSize;
}

void DataRow::copy(const std::vector<ExecutionResult>& results, std::byte* pData) {
	assert(results.size() == m_types.size());
	for(size_t i=0;i<results.size();++i) {
		if (auto iter = m_typeOperations.find(m_types[i]); iter != m_typeOperations.end()) {
			auto writeFn = std::get<WriteFn>(iter->second);
			writeFn(pData, results[i]);
			auto sizeFn = std::get<Size1Fn>(iter->second);
			pData +=sizeFn(pData);
		} else{
			assert(0);
		}
	}
}

