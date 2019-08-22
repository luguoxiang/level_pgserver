#include "common/ConfigException.h"
#include <glog/logging.h>

ConfigException::ConfigException(const std::string& sMsg) :m_sErrMsg(sMsg) {

}

