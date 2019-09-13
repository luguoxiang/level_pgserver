#pragma once

#include <absl/strings/str_cat.h>
#include <exception>


class ExecutionException: public std::exception {
public:
	ExecutionException(const std::string& sMsg);
	const char * what() const throw() override {
		return m_sErrMsg.c_str();
	}
private:
	std::string m_sErrMsg;
};

#define EXECUTION_ERROR(args...) {auto sError = absl::StrCat(args);LOG(ERROR)<<sError;throw ExecutionException(sError);}


