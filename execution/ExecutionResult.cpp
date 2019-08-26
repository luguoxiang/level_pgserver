#include "ExecutionResult.h"
#include "ExecutionException.h"
#include "ParseTools.h"
#include "DBDataTypeHandler.h"

void ExecutionResult::div(size_t value, DBDataType type) {
	if (value == 0) {
		EXECUTION_ERROR("Divide zero");
	}
	DBDataTypeHandler::getHandler(type)->div(*this, value);
}

void ExecutionResult::add(const ExecutionResult& result, DBDataType type) {
	try {
		DBDataTypeHandler::getHandler(type)->add(*this, result);
	} catch (const std::bad_variant_access& e) {
		EXECUTION_ERROR(e.what());
	}
}

int ExecutionResult::compare(const ExecutionResult& result,
		DBDataType type) const {
	try {
		return DBDataTypeHandler::getHandler(type)->compare(*this, result);
	} catch (const std::bad_variant_access& e) {
		EXECUTION_ERROR(e.what());
	}
}

int ExecutionResult::compare(const ParseNode* pValue, DBDataType type) const {
	try {
		ExecutionResult result;
		DBDataTypeHandler::getHandler(type)->fromNode(pValue, result);
		return DBDataTypeHandler::getHandler(type)->compare(*this, result);
	} catch (const std::bad_variant_access& e) {
		EXECUTION_ERROR(e.what());
	}
}

