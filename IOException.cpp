#include "IOException.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <cassert>
#include <glog/logging.h>

IOException::IOException(const std::string& sValue, const std::string& sIP) {
	m_sErrMsg.append(sValue);
	addErrorNo();

	m_sErrMsg.append(":");
	m_sErrMsg.append(sIP);

	LOG(ERROR)<<m_sErrMsg;
}

IOException::IOException(const std::string& sValue) {
	m_sErrMsg.append(sValue);
	addErrorNo();

	LOG(ERROR)<<m_sErrMsg;
}
