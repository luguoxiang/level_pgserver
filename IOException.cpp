#include "IOException.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <cassert>
#include <glog/logging.h>

IOException::IOException(const std::string& sValue) {
	m_sErrMsg.append(sValue);
	addErrorNo();

	LOG(ERROR)<<m_sErrMsg;
}
