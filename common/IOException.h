#pragma once

#include <string>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "common/Exception.h"

class IOException: public Exception {
public:
	IOException(const char* pszValue);
	IOException(const char* pszValue, const char* pszIP);

	virtual ~IOException() {
	}

	std::string what() const override {
		return m_sErrMsg;
	}

	IOException& operator=(const IOException& ex) = delete;
	IOException(const IOException& ex) = delete;

private:
	void addErrorNo() {
		if (errno == 0)
			return;
		m_sErrMsg.append("(");
		m_sErrMsg.append(strerror(errno));
		m_sErrMsg.append(")");
		errno = 0;
	}

	std::string m_sErrMsg;
};
