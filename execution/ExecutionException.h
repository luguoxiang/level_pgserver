#pragma once
#include <string>
#include "common/Exception.h"

class ExecutionException: public Exception
{
public:
	ExecutionException(const char* pszMsg, bool appendOBError);
	virtual ~ExecutionException();
	const char* what() const
	{
		return m_sErrMsg.c_str();
	}
private:
	std::string m_sErrMsg;
};

