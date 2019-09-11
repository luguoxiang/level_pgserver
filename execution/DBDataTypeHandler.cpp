#include "DBDataTypeHandler.h"
#include "common/ParseTools.h"
#include <limits>
#include <absl/strings/numbers.h>

std::map<DBDataType, std::unique_ptr<DBDataTypeHandler>> DBDataTypeHandler::m_typeHandlers;

namespace {

template<typename Type>
class IntDBDataTypeHandler: public DBDataTypeHandler {
private:
	void checkValue(int64_t value) {
		static_assert(std::numeric_limits<Type>::is_signed);

		if (value < std::numeric_limits<Type>::min() ||
		               value > std::numeric_limits<Type>::max() ){
			PARSE_ERROR("integer ", value, " exceed data type range");
		}
	}
public:
	IntDBDataTypeHandler(const std::string& name) : DBDataTypeHandler(name) {};
	size_t getSize(const ExecutionResult& result) override {
		return sizeof(Type);
	}

	size_t getSize(const std::byte* pData) override {
		return sizeof(Type);
	}

	void setToMin(ExecutionResult& result) override {
		result.setInt(std::numeric_limits<Type>::min());
	}

	void setToMax(ExecutionResult& result) override {
		result.setInt(std::numeric_limits<Type>::max());
	}

	void read(const std::byte* pData, ExecutionResult& result) override {
		const Type v = *reinterpret_cast<const Type*>(pData);
		result.setInt(v);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		int64_t value = result.getInt();
		checkValue(value);
		*reinterpret_cast<Type*>(pData) = value;
	}

	void fromString(const std::string_view s, ExecutionResult& result) override{
		auto value = stringToInt(s);
		result.setInt(value);
	}

	int64_t stringToInt(const std::string_view s) {
		if(s.empty()) {
			return 0;
		}else {
			int64_t value;
			if(!absl::SimpleAtoi(s, &value)) {
				PARSE_ERROR("Could not convert to int:", s);
			}
			checkValue(value);
			return value;
		}
	}

	void fromNode(const ParseNode* pValue, ExecutionResult& result) override {
		int64_t value = 0;
		switch (pValue->m_type) {
		case NodeType::INT:
			value = stringToInt(pValue->getString());
			break;
		case NodeType::PARAM: {
			switch(pValue->getOp()) {
			case Operation::TEXT_PARAM:{
				value = stringToInt(pValue->getString());
				break;
			}
			case Operation::BINARY_PARAM:
				value = Tools::binaryToInt(pValue->getString());
				break;
			case Operation::UNBOUND_PARAM:
				EXECUTION_ERROR("parameter unbound");
				break;
			default:
				assert(0);
				break;
			}
			break;
		}
		default:
			EXECUTION_ERROR("wrong const value type:", pValue->m_sExpr);
		}
		checkValue(value);
		result.setInt(value);
	}
	void div(ExecutionResult& result, size_t value) override {
		auto v = result.getInt();
		v = v / value;
		result.setInt(v);
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		int64_t value = result.getInt() + add.getInt();
		checkValue(value);
		result.setInt(value);

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

};

template<typename Type>
class FloatDBDataTypeHandler: public DBDataTypeHandler {
private:
	void checkValue(double value) {
		static_assert(! std::numeric_limits<Type>::is_integer );
		if( (value > 0 ? value  : -value) > std::numeric_limits<Type>::max() ) {
			PARSE_ERROR("float ", value, " exceed data type range");
		}
	}
public:
	FloatDBDataTypeHandler(const std::string& name) : DBDataTypeHandler(name) {};
	size_t getSize(const ExecutionResult& result) override {
		return sizeof(Type);
	}

	virtual size_t getSize(const std::byte* pData) override {
		return sizeof(Type);
	}

	void setToMin(ExecutionResult& result) override {
		result.setDouble(std::numeric_limits<Type>::lowest());
	}

	void setToMax(ExecutionResult& result) override {
		result.setDouble(std::numeric_limits<Type>::max());
	}

	void read(const std::byte* pData, ExecutionResult& result) override {
		const Type v = *reinterpret_cast<const Type*>(pData);
		result.setDouble(v);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		double value = result.getDouble();
		checkValue(value);
		*reinterpret_cast<Type*>(pData) = value;
	}
	double stringToDouble(const std::string_view s) {
		if(s.empty()) {
			return 0;
		}else {
			double value;
			if (!absl::SimpleAtod(s, &value)) {
				PARSE_ERROR("Could not convert to double:", s);
			}
			return value;
		}
	}
	void fromString(const std::string_view s, ExecutionResult& result) override{
		auto value = stringToDouble(s);
		checkValue(value);
		result.setDouble(value);
	}
	void fromNode(const ParseNode* pValue, ExecutionResult& result) override {
		double value = 0;
		switch (pValue->m_type) {
		case NodeType::INT:
		case NodeType::FLOAT:
			value = stringToDouble(pValue->getString());
			break;
		case NodeType::PARAM: {
			switch(pValue->getOp()) {
			case Operation::TEXT_PARAM: {
				value = stringToDouble(pValue->getString());
				break;
			}
			case Operation::BINARY_PARAM:
				value = Tools::binaryToDouble(pValue->getString());
				break;
			case Operation::UNBOUND_PARAM:
				EXECUTION_ERROR("parameter unbound");
				break;
			default:
				assert(0);
				break;
			}
			break;
		}
		default:
			PARSE_ERROR("wrong const value type:", pValue->m_sExpr);
		}
		checkValue(value);
		result.setDouble(value);
	}

	void div(ExecutionResult& result, size_t value) override {
		auto v = result.getDouble();
		v = v / value;
		result.setDouble(v);
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		double value = result.getDouble() + add.getDouble();
		checkValue(value);
		result.setDouble(value);

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
};
class StringDBDataTypeHandler: public DBDataTypeHandler {
public:
	StringDBDataTypeHandler(const std::string& name) : DBDataTypeHandler(name) {};
	size_t getSize(const ExecutionResult& result) override {
		if(result.isMaxString()) {
			return sizeof(uint16_t);
		}
		return result.getString().length() + sizeof(uint16_t);
	}
	void setToMin(ExecutionResult& result) override {
		result.setStringView("");
	}

	void setToMax(ExecutionResult& result) override {
		result.setMaxString();
	}

	virtual size_t getSize(const std::byte* pData) override {
		size_t len = *(reinterpret_cast<const uint16_t*>(pData)) + sizeof(uint16_t);
		if(len == std::numeric_limits<uint16_t>::max()) {
			return sizeof(uint16_t);
		}
		return len;
	}
	void read(const std::byte* pData, ExecutionResult& result) override {
		size_t len = *(reinterpret_cast<const uint16_t*>(pData));
		if(len == std::numeric_limits<uint16_t>::max()) {
			result.setMaxString();
			return;
		}
		pData += sizeof(uint16_t);
		std::string_view s(reinterpret_cast<const char*>(pData), len);
		result.setStringView(s);
	}
	void write(std::byte* pData, const ExecutionResult& result) override {
		if(result.isMaxString()) {
			*reinterpret_cast<uint16_t*>(pData) = std::numeric_limits<uint16_t>::max();
			return;
		}
		auto s = result.getString();
		auto size = s.length();
		if (size >= std::numeric_limits<uint16_t>::max()) {
			EXECUTION_ERROR("too large string value");
		}

		*reinterpret_cast<uint16_t*>(pData) = size;
		pData += sizeof(uint16_t);
		auto pSrc = reinterpret_cast<const std::byte*>(s.data());
		std::copy(pSrc, pSrc + size, pData);
	}

	void fromString(const std::string_view s, ExecutionResult& result) override{
		result.setStringView(s);
	}
	void fromNode(const ParseNode* pValue, ExecutionResult& result) override {
		switch (pValue->m_type) {
		case NodeType::STR:
		case NodeType::BINARY:
			break;
		case NodeType::PARAM:
			switch(pValue->getOp()) {
			case Operation::TEXT_PARAM:
				break;
			case Operation::UNBOUND_PARAM:
				EXECUTION_ERROR("parameter unbound");
				break;
			default:
				EXECUTION_ERROR("wrong bind param mode");
				break;
			}
			break;
		default:
			EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr,
					", expect string");
		}
		result.setStringView(pValue->getString());

	}
	void div(ExecutionResult& result, size_t value) override {
		EXECUTION_ERROR("div is not supported for string");
	}
	void add(ExecutionResult& result, const ExecutionResult& add) override {
		EXECUTION_ERROR("add is not supported for string");
	}
	int compare(const ExecutionResult& a, const ExecutionResult& b) override {
		if( a.isMaxString()) {
			if( b.isMaxString()) {
				return 0;
			} else {
				return 1;
			}
		} else if( b.isMaxString()){
			return -1;
		}
		return a.getString().compare(b.getString());
	}
};

}

class DatetimeDBDataTypeHandler: public IntDBDataTypeHandler<int64_t> {
public:
	DatetimeDBDataTypeHandler(const std::string& name) : IntDBDataTypeHandler<int64_t>(name) {};

	int64_t timeToInt(const std::string_view sTime) {
		if(sTime.empty()) {
			return 0;
		}else {
			int64_t iValue = parseTime(sTime);
			if (iValue <= 0) {
				EXECUTION_ERROR("Wrong Time Format:", sTime);
			}
			return iValue;
		}
	}

	void fromString(const std::string_view s, ExecutionResult& result) override{
		result.setInt(timeToInt(s));
	}

	void fromNode(const ParseNode* pValue, ExecutionResult& result) override {
		switch (pValue->m_type) {
		case NodeType::DATE:
			result.setInt(timeToInt(pValue->getString()));
			break;
		case NodeType::PARAM:
			switch(pValue->getOp()) {
			case Operation::TEXT_PARAM: {
				auto sValue = pValue->getString();
				result.setInt(timeToInt(sValue));
				break;
			}
			case Operation::UNBOUND_PARAM:
				EXECUTION_ERROR("parameter unbound");
				break;
			case Operation::BINARY_PARAM:
				EXECUTION_ERROR("Wrong bind parameter mode for datetime");
				break;
			default:
				break;
			}
			break;
		default:
			PARSE_ERROR("wrong const value type:", pValue->m_sExpr);
		}
	}
};
void DBDataTypeHandler::init() {
	m_typeHandlers[DBDataType::INT16] = std::make_unique<
			IntDBDataTypeHandler<int16_t>>("int16");
	m_typeHandlers[DBDataType::INT32] = std::make_unique<
			IntDBDataTypeHandler<int32_t>>("int32");
	m_typeHandlers[DBDataType::INT64] = std::make_unique<
			IntDBDataTypeHandler<int64_t>>("int64");

	m_typeHandlers[DBDataType::DATE] = std::make_unique<DatetimeDBDataTypeHandler>("date");
	m_typeHandlers[DBDataType::DATETIME] =std::make_unique<DatetimeDBDataTypeHandler>("datetime");

	m_typeHandlers[DBDataType::FLOAT] = std::make_unique<
			FloatDBDataTypeHandler<float>>("float");
	m_typeHandlers[DBDataType::DOUBLE] = std::make_unique<
			FloatDBDataTypeHandler<double>>("double");

	m_typeHandlers[DBDataType::STRING] = std::make_unique<StringDBDataTypeHandler>("varchar");
	m_typeHandlers[DBDataType::BYTES] =  std::make_unique<StringDBDataTypeHandler>("bytes");
}
