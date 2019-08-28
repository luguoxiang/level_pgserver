#include "ExecutionResult.h"
#include "ParseTools.h"
#include "DBDataTypeHandler.h"

void ExecutionResult::div(size_t value, DBDataType type) {
	if (value == 0) {
		EXECUTION_ERROR("Divide zero");
	}
	DBDataTypeHandler::getHandler(type)->div(*this, value);
}

void ExecutionResult::add(const ExecutionResult& result, DBDataType type) {
	DBDataTypeHandler::getHandler(type)->add(*this, result);
}

int ExecutionResult::compare(const ExecutionResult& result,
		DBDataType type) const {
	return DBDataTypeHandler::getHandler(type)->compare(*this, result);
}


