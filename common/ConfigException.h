#pragma once

#include <string>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "common/Exception.h"

class ConfigException: public Exception
{
public:
	ConfigException(const char* pszMsg, ...);

	virtual ~ConfigException()
	{
	}

	const char* what() const
	{
		return m_sErrMsg.c_str();
	}

private:
	ConfigException& operator=(const ConfigException& ex);
	ConfigException(const ConfigException& ex);

	std::string m_sErrMsg;
};
