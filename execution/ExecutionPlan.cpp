#include "ExecutionPlan.h"
#include <cassert>
#include "execution/ParseTools.h"
#include "execution/ExecutionException.h"
#include "common/ParseException.h"
#include "common/Log.h"
namespace {
int string_compare(const char* a, size_t la, const char* b, size_t lb) {
	if (la == lb) {
		return strncmp(a, b, la);
	} else if (la < lb) {
		int n = strncmp(a, b, la);
		if (n != 0)
			return n;
		return -1;
	} else // if(la > lb)
	{
		int n = strncmp(a, b, lb);
		if (n != 0)
			return n;
		return 1;
	}
}
}

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

void ExecutionPlan::getInfoString(char* szBuf, int len) {
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
	case TYPE_STRING:
	case TYPE_BYTES:
	case TYPE_DATE:
	case TYPE_DATETIME:
		return false;
	case TYPE_DOUBLE: {
		m_value.m_dResult /= value;
		return true;
	}
	case TYPE_INT16:
	case TYPE_INT32:
	case TYPE_INT64: {
		m_value.m_lResult /= value;
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
	case TYPE_STRING:
	case TYPE_BYTES:
		return false;
	case TYPE_DOUBLE: {
		m_value.m_dResult += result.m_value.m_dResult;
		return true;
	}
	case TYPE_INT16:
	case TYPE_INT32:
	case TYPE_INT64:
	case TYPE_DATE:
	case TYPE_DATETIME: {
		m_value.m_lResult += result.m_value.m_lResult;
		return true;
	}
	default:
		PARSE_ERROR("Unsupported data type!")
		;
		return 0;
	}
}

int ExecutionPlan::ResultInfo::compare(const ResultInfo& result,
		DBDataType type) {
	if (m_bNull && result.m_bNull)
		return 0;
	if (m_bNull)
		return -1;
	if (result.m_bNull)
		return 1;

	switch (type) {
	case TYPE_STRING:
	case TYPE_BYTES:
		return string_compare(m_value.m_pszResult, m_len,
				result.m_value.m_pszResult, result.m_len);
	case TYPE_DOUBLE: {
		double aa = m_value.m_dResult;
		double bb = result.m_value.m_dResult;
		if (aa == bb)
			return 0;
		else if (aa < bb)
			return -1;
		else
			return 1;
	}
	case TYPE_INT8:
	case TYPE_INT16:
	case TYPE_INT32:
	case TYPE_INT64:
	case TYPE_DATE:
	case TYPE_DATETIME: {
		int64_t aa = m_value.m_lResult;
		int64_t bb = result.m_value.m_lResult;
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
		DBDataType type) {
	if (m_bNull) {
		return pValue->m_iType == NULL_NODE ? 0 : -1;
	}
	switch (type) {
	case TYPE_INT16:
	case TYPE_INT32:
	case TYPE_INT64: {
		if (pValue->m_iType != INT_NODE) {
			PARSE_ERROR("Wrong data type for %s, expect int",
					pValue->m_pszValue);
		}
		int64_t a = m_value.m_lResult;
		int64_t b = pValue->m_iValue;
		if (a == b)
			return 0;
		else if (a < b)
			return -1;
		else
			return 1;
	}
	case TYPE_STRING: {
		if (pValue->m_iType != STR_NODE) {
			PARSE_ERROR("Wrong data type for %s, expect string",
					pValue->m_pszValue);
		}
		return string_compare(m_value.m_pszResult, m_len, pValue->m_pszValue,
				pValue->m_iValue);
	}
		//case TYPE_DATE:
		//case TYPE_DATETIME:
		//case TYPE_DOUBLE:
		//case TYPE_BYTES:
	default:
		PARSE_ERROR("Unsupported compare for %s", pValue->m_pszValue)
		;
		break;
	};
	return 0;
}

