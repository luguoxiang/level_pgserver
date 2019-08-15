#pragma once
#include <string>
#include "common/Exception.h"

class ExecutionException: public Exception {
public:
	ExecutionException(const std::string& sMsg);
	virtual ~ExecutionException();
	std::string what() const {
		return m_sErrMsg;
	}
private:
	std::string m_sErrMsg;
};

