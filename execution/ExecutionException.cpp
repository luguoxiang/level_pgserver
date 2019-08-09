#include "ExecutionException.h"

ExecutionException::ExecutionException(const char* pszMsg, bool appendOBError) :
		m_sErrMsg(pszMsg) {
}

ExecutionException::~ExecutionException() {
}
