#ifndef IO_EXCEPTION_H
#define IO_EXCEPTION_H

#include <string>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "common/Exception.h"

class IOException: public Exception
{
public:
	IOException(const char* pszValue);
	IOException(const char* pszValue, const char* pszIP);

	virtual ~IOException()
	{
	}

	const char* what() const
	{
		return m_sErrMsg.c_str();
	}

private:
	IOException& operator=(const IOException& ex);
	IOException(const IOException& ex);

	void addErrorNo()
	{
		if (errno == 0)
			return;
		m_sErrMsg.append("(");
		m_sErrMsg.append(strerror(errno));
		m_sErrMsg.append(")");
		errno = 0;
	}

	std::string m_sErrMsg;
};
#endif  // IO_EXCEPTION_H
