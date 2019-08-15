#include "ExecutionException.h"
#include <glog/logging.h>

ExecutionException::ExecutionException(const std::string& sMsg) :
		m_sErrMsg(sMsg) {
	LOG(ERROR) << sMsg;
}

ExecutionException::~ExecutionException() {
}
