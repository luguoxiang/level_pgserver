#pragma once

#include <glog/logging.h>
#include <exception>
#include <absl/strings/str_cat.h>


class ConfigException: public std::exception {
public:
	ConfigException(const std::string& sMsg);

	const char * what() const throw() override {
		return m_sErrMsg.c_str();
	}

private:
	std::string m_sErrMsg;
};

#define CONFIG_ERROR(args...) {auto sError = absl::StrCat(args);LOG(ERROR)<<sError;throw ConfigException(sError);}


