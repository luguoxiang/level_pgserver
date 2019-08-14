#include "ExecutionException.h"

ExecutionException::ExecutionException(const std::string& sMsg, bool appendOBError) :
		m_sErrMsg(sMsg) {
}

ExecutionException::~ExecutionException() {
}
