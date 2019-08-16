#include "ExecutionPlan.h"
#include <cassert>
#include <sstream>
#include "execution/ParseTools.h"
#include "execution/ExecutionException.h"
#include "common/ParseException.h"


ExecutionPlan::ExecutionPlan(PlanType type) :
		m_type(type) {
}
ExecutionPlan::~ExecutionPlan() {
}

void ExecutionPlan::begin() {
}

void ExecutionPlan::end() {
}

bool ExecutionPlan::next() {
	return false;
}

void ExecutionPlan::cancel() {
}



int ExecutionPlan::getResultColumns() {
	return 0;
}

void ExecutionPlan::getResult(size_t index, ResultInfo* pInfo) {
}

bool ExecutionPlan::ResultInfo::div(size_t value, DBDataType type) {
	if (value == 0) {
		throw new ExecutionException("Divide zero");
	}
	if (m_bNull) {
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
		throw new ParseException("Divide is not supported on current data type!");
		return 0;
	}
}

bool ExecutionPlan::ResultInfo::add(const ResultInfo& result, DBDataType type) {
	switch (type) {
	case DBDataType::DOUBLE: {
		m_result = getDouble() + result.getDouble();
		return true;
	}
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

int ExecutionPlan::ResultInfo::compare(const ResultInfo& result,
		DBDataType type) const {
	if (m_bNull && result.m_bNull)
		return 0;
	if (m_bNull)
		return -1;
	if (result.m_bNull)
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
	case DBDataType::INT8:
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64:
	case DBDataType::DATE:
	case DBDataType::DATETIME: {
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
		throw new ParseException("Unsupported compare data type!");
		return 0;
	}
}

int ExecutionPlan::ResultInfo::compare(const ParseNode* pValue,
		DBDataType type) const {
	if (m_bNull) {
		return pValue->m_type == NodeType::NONE ? 0 : -1;
	}
	switch (type) {
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		if (pValue->m_type != NodeType::INT) {
			std::ostringstream os;
			os << "Wrong data type for " << pValue->m_sExpr << ", expect int";
			throw new ParseException(os.str());
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
			std::ostringstream os;
			os << "Wrong data type for " << pValue->m_sExpr << ", expect string";
			throw new ParseException(os.str());
		}
		return getString() == pValue->m_sValue;
	}
		//case DBDataType::DATE:
		//case DBDataType::DATETIME:
		//case DBDataType::DOUBLE:
		//case DBDataType::BYTES:
	default:
		std::ostringstream os;
		os << "Unsupported compare for " << pValue->m_sExpr;
		throw new ParseException(os.str());
	};
	return 0;
}

