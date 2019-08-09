#include "common/ConfigException.h"
#include "common/Log.h"

#include <stdarg.h>
ConfigException::ConfigException(const char* pszMsg, ...) {
	va_list arg;
	va_start(arg, pszMsg);

	char szBuf[100];
	vsnprintf(szBuf, 100, pszMsg, arg);
	m_sErrMsg = szBuf;

	va_end(arg);
	LOG(ERROR, m_sErrMsg.c_str());
}

