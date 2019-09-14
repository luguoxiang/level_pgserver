#pragma once

#include <string>
#include <string.h>
#include <netdb.h>
#include <errno.h>
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

#define IO_ERROR(args...) {auto sError = absl::StrCat(args);LOG(ERROR)<<sError;throw IOException(sError);}
