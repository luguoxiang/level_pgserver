#include "DBDataTypeHandler.h"
#include "ParseTools.h"

std::map<DBDataType, std::unique_ptr<DBDataTypeHandler>> DBDataTypeHandler::m_typeHandlers;

namespace {

template<typename Type>
class IntDBDataTypeHandler: public DBDataTypeHandler {
public:
	size_t getSize(const ExecutionResult& result) override {
		return sizeof(Type);
	}

	void read(const std::byte* pData, ExecutionResult& result) override {
		const Type v = *reinterpret_cast<const Type*>(pData);
		result.setInt(v);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		*reinterpret_cast<Type*>(pData) = result.getInt();
	}

	void div(ExecutionResult& result, size_t value) override {
		auto v = result.getInt();
		v = v / value;
		result.setInt(v);
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		result.setInt(result.getInt() + add.getInt());

	}
	int compare(const ExecutionResult& a, const ExecutionResult& b) override {
		auto aa = a.getInt();
		auto bb = b.getInt();
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	int compare(const ExecutionResult& result, const ParseNode* pValue) override {
		int64_t b = 0;
		switch (pValue->m_type) {
		case NodeType::INT:
			b = pValue->m_iValue;
			break;
		case NodeType::PARAM:
			b = Tools::bindParamToInt(pValue->m_iValue, pValue->m_sValue);
			break;
		default:
			EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr,
					", expect int");
		}
		int64_t a = result.getInt();
		if (a == b)
			return 0;
		else if (a < b)
			return -1;
		else
			return 1;
	}
};

template<typename Type>
class FloatDBDataTypeHandler: public DBDataTypeHandler {
public:
	size_t getSize(const ExecutionResult& result) override {
		return sizeof(Type);
	}

	void read(const std::byte* pData, ExecutionResult& result) override {
		const Type v = *reinterpret_cast<const Type*>(pData);
		result.setDouble(v);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		*reinterpret_cast<Type*>(pData) = result.getDouble();
	}

	void div(ExecutionResult& result, size_t value) override {
		auto v = result.getDouble();
		v = v / value;
		result.setDouble(v);
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		result.setDouble(result.getDouble() + add.getDouble());

	}
	int compare(const ExecutionResult& a, const ExecutionResult& b) override {
		auto aa = a.getDouble();
		auto bb = b.getDouble();
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	int compare(const ExecutionResult& result, const ParseNode* pValue) override {
		double b = 0;
		switch (pValue->m_type) {
		case NodeType::INT:
			b = pValue->m_iValue;
			break;
		case NodeType::FLOAT:
			b = Tools::toDouble(pValue->m_sValue);
			break;
		case NodeType::PARAM:
			b = Tools::bindParamToDouble(pValue->m_iValue,
					pValue->m_sValue);
			break;
		default:
			EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr,
					", expect double");
		}
		auto a = result.getDouble();
		if (a == b)
			return 0;
		else if (a < b)
			return -1;
		else
			return 1;
	}
};
class StringDBDataTypeHandler: public DBDataTypeHandler {
	size_t getSize(const ExecutionResult& result) override {
		return result.getString().length() + sizeof(uint16_t);
	}

	void read(const std::byte* pData, ExecutionResult& result) override {
		size_t len = *(reinterpret_cast<const uint16_t*>(pData));
		pData += sizeof(uint16_t);
		std::string_view s(reinterpret_cast<const char*>(pData), len);
		result.setStringView(s);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		auto s = result.getString();
		auto size = s.length();
		if (size > std::numeric_limits < uint16_t > ::max()) {
			EXECUTION_ERROR("too large string value");
		}

		*reinterpret_cast<uint16_t*>(pData) = size;
		pData += sizeof(uint16_t);
		auto pSrc = reinterpret_cast<const std::byte*>(s.data());
		std::copy(pSrc, pSrc + size, pData);
	}

	void div(ExecutionResult& result, size_t value) override {
		EXECUTION_ERROR("div is not supported for string");
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		EXECUTION_ERROR("add is not supported for string");
	}
	int compare(const ExecutionResult& a, const ExecutionResult& b) override {
		return a.getString().compare(b.getString());
	}
	int compare(const ExecutionResult& result, const ParseNode* pValue)
			override {
		switch (pValue->m_type) {
		case NodeType::STR:
			break;
		case NodeType::PARAM:
			if (pValue->m_iValue == PARAM_TEXT_MODE) {
				break;
			}
			//fall through
		default:
			EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr,
					", expect string");
		}
		return result.getString().compare(pValue->m_sValue);
	}
};

}
void DBDataTypeHandler::init() {
	m_typeHandlers[DBDataType::INT8] = std::make_unique<
			IntDBDataTypeHandler<int8_t>>();
	m_typeHandlers[DBDataType::INT16] = std::make_unique<
			IntDBDataTypeHandler<int16_t>>();
	m_typeHandlers[DBDataType::INT32] = std::make_unique<
			IntDBDataTypeHandler<int32_t>>();
	m_typeHandlers[DBDataType::INT64] = std::make_unique<
			IntDBDataTypeHandler<int64_t>>();

	m_typeHandlers[DBDataType::DATE] = std::make_unique<
			IntDBDataTypeHandler<int64_t>>();
	m_typeHandlers[DBDataType::DATETIME] =std::make_unique<
			IntDBDataTypeHandler<int64_t>>();

	m_typeHandlers[DBDataType::FLOAT] = std::make_unique<
			FloatDBDataTypeHandler<float>>();
	m_typeHandlers[DBDataType::DOUBLE] = std::make_unique<
			FloatDBDataTypeHandler<double>>();

	m_typeHandlers[DBDataType::STRING] = std::make_unique<StringDBDataTypeHandler>();
	m_typeHandlers[DBDataType::BYTES] =  std::make_unique<StringDBDataTypeHandler>();
}
