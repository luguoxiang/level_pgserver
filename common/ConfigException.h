#pragma once


#include "common/Exception.h"



class ConfigException: public Exception {
public:
	ConfigException(const std::string& sMsg);

	virtual ~ConfigException() {
	}

	std::string what() const override {
		return m_sErrMsg;
	}

	ConfigException& operator=(const ConfigException& ex) = delete;
	ConfigException(const ConfigException& ex) = delete;

private:
	std::string m_sErrMsg;
};

#define CONFIG_ERROR(args...) {auto sError = ConcateToString(args);LOG(ERROR)<<sError;throw new ConfigException(sError);}


