#pragma once

#include <string>
#include <string.h>
#include <exception>
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

class IOException: public std::exception {
public:
	IOException(const std::string& sValue);

	const char * what() const throw() override {
		return m_sErrMsg.c_str();
	}

private:

	std::string m_sErrMsg;
};

#define IO_ERROR(...) {auto sError = absl::StrCat(__VA_ARGS__);LOG(ERROR)<<sError;throw IOException(sError);}
