#pragma once

#include <absl/strings/str_cat.h>
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

#define EXECUTION_ERROR(args...) {auto sError = absl::StrCat(args);LOG(ERROR)<<sError;throw new ExecutionException(sError);}


