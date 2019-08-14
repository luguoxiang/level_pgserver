#include "ExecutionPlan.h"
#include <cassert>
#include "execution/ParseTools.h"
#include "execution/ExecutionException.h"
#include "common/ParseException.h"
#include "common/Log.h"


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
		EXEC_ERROR("Divide zero", false);
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
		m_dResult /= value;
		return true;
	}
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64: {
		m_lResult /= value;
		return true;
	}
	default:
		PARSE_ERROR("Divide is not supported on current data type!")
		;
		return 0;
	}
}

bool ExecutionPlan::ResultInfo::add(const ResultInfo& result, DBDataType type) {
	if (m_bNull) {
		*this = result;
		return true;
	}
	if (result.m_bNull)
		return true;

	switch (type) {
	case DBDataType::STRING:
	case DBDataType::BYTES:
		return false;
	case DBDataType::DOUBLE: {
		m_dResult += result.m_dResult;
		return true;
	}
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64:
	case DBDataType::DATE:
	case DBDataType::DATETIME: {
		m_lResult += result.m_lResult;
		return true;
	}
	default:
		PARSE_ERROR("Unsupported data type!")
		;
		return 0;
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
		return (m_sResult == result.m_sResult);
	case DBDataType::DOUBLE: {
		double aa = m_dResult;
		double bb = result.m_dResult;
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
		int64_t aa = m_lResult;
		int64_t bb = result.m_lResult;
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	default:
		PARSE_ERROR("Unsupported compare data type!")
		;
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
			PARSE_ERROR("Wrong data type for %s, expect int",
					pValue->m_sValue.c_str());
		}
		int64_t a = m_lResult;
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
			PARSE_ERROR("Wrong data type for %s, expect string",
					pValue->m_sValue.c_str());
		}
		return m_sResult == pValue->m_sValue;
	}
		//case DBDataType::DATE:
		//case DBDataType::DATETIME:
		//case DBDataType::DOUBLE:
		//case DBDataType::BYTES:
	default:
		PARSE_ERROR("Unsupported compare for %s", pValue->m_sValue.c_str())
		;
		break;
	};
	return 0;
}

