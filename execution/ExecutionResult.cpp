#include "ExecutionResult.h"
#include "ExecutionException.h"

bool ExecutionResult::div(size_t value, DBDataType type) {
	if (value == 0) {
		throw new ExecutionException("Divide zero");
	}
	if (isNull()) {
		return true;
	}
	switch (type) {
	case DBDataType::STRING:
	case DBDataType::BYTES:
	case DBDataType::DATE:
	case DBDataType::DATETIME:
		return false;
	case DBDataType::DOUBLE: {
		auto v = getDouble();
		v = v / value;
		m_result = v;
		return true;
	}
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		auto v = getInt();
		v = v/ value;
		m_result = v;
		return true;
	}
	default:
		throw new ExecutionException("Divide is not supported on current data type!");
		return 0;
	}
}

bool ExecutionResult::add(const ExecutionResult& result, DBDataType type) {
	switch (type) {
	case DBDataType::DOUBLE: {
		m_result = getDouble() + result.getDouble();
		return true;
	}
	case DBDataType::DATE:
	case DBDataType::DATETIME:
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		m_result = getInt() + result.getInt();
		return true;
	}
	default:
		return false;
	}
}

int ExecutionResult::compare(const ExecutionResult& result,
		DBDataType type) const {
	if (isNull() && result.isNull())
		return 0;
	if (isNull())
		return -1;
	if (result.isNull())
		return 1;

	switch (type) {
	case DBDataType::STRING:
	case DBDataType::BYTES:
		return (getString() == result.getString());
	case DBDataType::DOUBLE: {
		double aa = getDouble();
		double bb = result.getDouble();
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	case DBDataType::DATE:
	case DBDataType::DATETIME:
	case DBDataType::INT8:
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		int64_t aa = getInt();
		int64_t bb = result.getInt();
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	default:
		throw new ExecutionException("Unsupported compare data type!");
		return 0;
	}
}

int ExecutionResult::compare(const ParseNode* pValue,
		DBDataType type) const {
	if (isNull()) {
		return pValue->m_type == NodeType::NONE ? 0 : -1;
	}
	switch (type) {
	case DBDataType::DATE:
	case DBDataType::DATETIME:
	case DBDataType::INT8:
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		if (pValue->m_type != NodeType::INT) {
			throw new ExecutionException(ConcateToString("Wrong data type for ", pValue->m_sExpr, ", expect int"));
		}
		int64_t a = getInt();
		int64_t b = pValue->m_iValue;
		if (a == b)
			return 0;
		else if (a < b)
			return -1;
		else
			return 1;
	}
	case DBDataType::STRING: {
		if (pValue->m_type != NodeType::STR) {
			throw new ExecutionException(ConcateToString("Wrong data type for ", pValue->m_sExpr, ", expect string"));
		}
		return getString() == pValue->m_sValue;
	}
		//case DBDataType::DOUBLE:
		//case DBDataType::BYTES:
	default:
		throw new ExecutionException(ConcateToString("Unsupported compare for ", pValue->m_sExpr));
	};
	return 0;
}

