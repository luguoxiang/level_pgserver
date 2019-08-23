#include "ExecutionResult.h"
#include "ExecutionException.h"
#include "execution/ParseTools.h"

std::map<DBDataType, ExecutionResult::TypeOperationTuple> ExecutionResult::m_typeOperations;

void ExecutionResult::init() {
	auto intOperations =  std::make_tuple(
		DivFn{[](ExecutionResult& result, size_t value) {
			auto v = result.getInt();
			v = v/value;
			result.setInt(v);
		}},
		AddFn{[](ExecutionResult& result, const ExecutionResult& add) {
			result.setInt(result.getInt() + add.getInt());
		}},
		Compare1Fn{[] (const ExecutionResult& a, const ExecutionResult& b) {
			auto aa = a.getInt();
			auto bb = b.getInt();
			if (aa == bb)
				return 0;
			else if (aa < bb)
				return -1;
			else
				return 1;
		}},
		Compare2Fn{[] (const ExecutionResult& result, const ParseNode* pValue) {
			auto b = pValue->m_iValue;
			switch(pValue->m_type) {
			case NodeType::INT:
				break;
			case NodeType::PARAM:
				b = Tools::bindParamToInt(pValue->m_iValue, pValue->m_sValue);
				break;
			default:
				EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr, ", expect int");
			}
			int64_t a = result.getInt();
			if (a == b)
				return 0;
			else if (a < b)
				return -1;
			else
				return 1;
		}}
	);

	m_typeOperations[DBDataType::INT8] = intOperations;
	m_typeOperations[DBDataType::INT16] = intOperations;
	m_typeOperations[DBDataType::INT32] = intOperations;
	m_typeOperations[DBDataType::INT64] = intOperations;

	m_typeOperations[DBDataType::DATE] = std::make_tuple(
			DivFn{nullptr},
			std::get<AddFn>(intOperations),
			std::get<Compare1Fn>(intOperations),
			std::get<Compare2Fn>(intOperations));
	m_typeOperations[DBDataType::DATETIME] = m_typeOperations[DBDataType::DATE];

	m_typeOperations[DBDataType::STRING] = std::make_tuple(
			DivFn{nullptr},
			AddFn{nullptr},
			Compare1Fn{[] (const ExecutionResult& a, const ExecutionResult& b) {
				return a.getString().compare(b.getString());
			}},
			Compare2Fn{[] (const ExecutionResult& result, const ParseNode* pValue) {
				switch(pValue->m_type) {
				case NodeType::STR:
					break;
				case NodeType::PARAM:
					if(pValue->m_iValue == PARAM_TEXT_MODE) {
						break;
					}
					//fall through
				default:
					EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr, ", expect string");
				}
				return result.getString().compare(pValue->m_sValue);
			}}
	);
	m_typeOperations[DBDataType::BYTES] = m_typeOperations[DBDataType::STRING];

	m_typeOperations[DBDataType::DOUBLE] =  std::make_tuple(
		DivFn{[](ExecutionResult& result, size_t value) {
			auto v = result.getDouble();
			v = v/value;
			result.setInt(v);
		}},
		AddFn{[](ExecutionResult& result, const ExecutionResult& add) {
			result.setDouble(result.getDouble() + add.getDouble());
		}},
		Compare1Fn{[] (const ExecutionResult& a, const ExecutionResult& b) {
				auto aa = a.getDouble();
				auto bb = b.getDouble();
				if (aa == bb)
					return 0;
				else if (aa < bb)
					return -1;
				else
					return 1;
			}},
			Compare2Fn{[] (const ExecutionResult& result, const ParseNode* pValue) {
				double b = 0;
				switch(pValue->m_type) {
				case NodeType::INT:
						b = pValue->m_iValue;
						break;
				case NodeType::FLOAT:
					b = Tools::toDouble(pValue->m_sValue);
					break;
				case NodeType::PARAM:
					b = Tools::bindParamToDouble(pValue->m_iValue, pValue->m_sValue);
					break;
				default:
					EXECUTION_ERROR("Wrong data type for ", pValue->m_sExpr, ", expect double");
				}
				double a = result.getDouble();
				if (a == b)
					return 0;
				else if (a < b)
					return -1;
				else
					return 1;
			}}
	);
	m_typeOperations[DBDataType::FLOAT] = m_typeOperations[DBDataType::DOUBLE];
}

void ExecutionResult::div(size_t value, DBDataType type) {
	if (value == 0) {
		EXECUTION_ERROR("Divide zero");
	}
	if (auto iter = m_typeOperations.find(type); iter != m_typeOperations.end() ){
		if (auto fn = std::get<DivFn>(iter->second); fn != nullptr) {
			fn(*this, value);
			return;
		}
	}
	EXECUTION_ERROR("Divide is not supported on target data type!");

}

void ExecutionResult::add(const ExecutionResult& result, DBDataType type) {
	if (auto iter = m_typeOperations.find(type); iter != m_typeOperations.end() ){
		if (auto fn = std::get<AddFn>(iter->second); fn != nullptr) {
			fn(*this, result);
			return;
		}
	}
	EXECUTION_ERROR("Add is not supported on target data type!");
}

int ExecutionResult::compare(const ExecutionResult& result,
		DBDataType type) const {
	if (auto iter = m_typeOperations.find(type); iter != m_typeOperations.end() ){
		if (auto fn = std::get<Compare1Fn>(iter->second); fn != nullptr) {
			return fn( *this, result);
		}
	}
	EXECUTION_ERROR("Compare is not supported on target data type!");
	return 0;
}

int ExecutionResult::compare(const ParseNode* pValue,
		DBDataType type) const {
	if (auto iter = m_typeOperations.find(type); iter != m_typeOperations.end() ){
		if (auto fn = std::get<Compare2Fn>(iter->second); fn != nullptr) {
			return fn(*this, pValue);
		}
	}
	EXECUTION_ERROR("Compare is not supported on target data type!");
	return 0;
}

