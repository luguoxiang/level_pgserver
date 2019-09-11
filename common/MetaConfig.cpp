#include "MetaConfig.h"
#include "common/ConfigException.h"
#include <absl/strings/str_split.h>
#include <fstream>
#include <string>
#include <regex>
#include <cassert>
#include <glog/logging.h>

MetaConfig::MetaConfig() {
	m_dataTypeMap["int16"] = DBDataType::INT16;
	m_dataTypeMap["int32"] = DBDataType::INT32;
	m_dataTypeMap["int64"] = DBDataType::INT64;
	m_dataTypeMap["float"] = DBDataType::FLOAT;
	m_dataTypeMap["double"] = DBDataType::DOUBLE;
	m_dataTypeMap["varchar"] = DBDataType::STRING;
	m_dataTypeMap["bytes"] = DBDataType::BYTES;
	m_dataTypeMap["datetime"] = DBDataType::DATETIME;
	m_dataTypeMap["date"] = DBDataType::DATE;
}

void MetaConfig::clean() {
	m_tableMap.clear();
}

void MetaConfig::addTable(TableInfo* pTable) {
	auto name = pTable->getName();
	if (getTableInfo(name) != nullptr) {
		CONFIG_ERROR("table ", name, " already defined!");
	}
	m_tableMap[name] = std::unique_ptr < TableInfo > (pTable);
	LOG(INFO)<< "add table " << name;
}

void MetaConfig::load(const std::string& sPath) {
	try {
		std::string line;

		std::ifstream infile(sPath);
		if (infile.fail()) {
			CONFIG_ERROR("reading config file ", sPath, " failed");
		}
		clean();
		TableInfo* pCurrentTable = nullptr;

		std::regex attributeRegex(R"(([^=\s]+)\s*=\s*([^=]+)\s*)");

		while (std::getline(infile, line)) {
			if (size_t pos = line.find("#"); pos != std::string::npos) {
				line = line.substr(0, pos);
			}
			if (line.find("[table]") != std::string::npos) {
				pCurrentTable = new TableInfo();
				continue;
			}

			std::smatch matches;
			if (std::regex_search(line, matches, attributeRegex)) {
				if (matches.size() == 3) {
					std::string sKey = matches[1];
					std::string sValue = matches[2];

					if (pCurrentTable == nullptr) {
						if (sKey == "worker_num") {
							m_iWorkerNum = std::stol(sValue);
							LOG(INFO)<< "worker_num = "<< m_iWorkerNum;

						} else if(sKey == "timeout") {
							m_iTimeout = std::stol(sValue);
							LOG(INFO) << "timeout = "<< m_iTimeout;

						} else if(sKey == "network_buffer_size") {
							m_iNetBuffer = std::stol(sValue) * 1024 * 1024;
							LOG(INFO) << "network_buffer_size = "<< m_iNetBuffer;

						} else if(sKey == "execution_buffer") {
							m_iExecBuffer = std::stol(sValue);
							LOG(INFO) << "execution_buffer = "<< m_iExecBuffer;

						} else {
							LOG(WARNING) << "Unknown config attribute "<< sKey;
						}
						continue;
					}
					if (sKey == "name") {
						pCurrentTable->setName(sValue);
						addTable(pCurrentTable);
					} else if (sKey == "column") {
						pCurrentTable->addColumn(this, sValue);
					} else if (sKey == "key") {
						std::vector<std::string_view> keys = absl::StrSplit(sValue, ",", absl::SkipWhitespace());
						for(auto sName : keys) {
							pCurrentTable->addKeyColumn(sName);
						}

						pCurrentTable->evaluate();
					} else {
						DLOG(INFO) <<"add attribute "<< sKey << " = " << sValue;
						pCurrentTable->addAttribute(sKey, sValue);
					}
				}
			}
		}

	} catch (const std::ifstream::failure& e) {
		CONFIG_ERROR("reading config file failure: ", e.what());
	}
}

