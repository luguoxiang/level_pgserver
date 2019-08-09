#include "IOException.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <cassert>
#include "common/Log.h"

IOException::IOException(const char* pszValue, const char* pszIP) {
	assert(pszIP);

	m_sErrMsg.append(pszValue);
	addErrorNo();

	m_sErrMsg.append(":");
	m_sErrMsg.append(pszIP);

	LOG(ERROR, m_sErrMsg.c_str());
}

IOException::IOException(const char* pszValue) {
	m_sErrMsg.append(pszValue);
	addErrorNo();

	LOG(ERROR, m_sErrMsg.c_str());
}
