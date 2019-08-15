#include "MetaConfig.h"
#include "common/Log.h"
#include "common/ConfigException.h"
#include <fstream>
#include <string>
#include <regex>
#include <cassert>

MetaConfig::MetaConfig() :
		m_iWorkerNum(20), m_iTimeout(60), m_iNetBuffer(1024 * 1024), m_iExecBuffer(
				64 * 1024 * 1024) {
	m_dataTypeMap["int8"] = DBDataType::INT8;
	m_dataTypeMap["int16"] = DBDataType::INT16;
	m_dataTypeMap["int32"] = DBDataType::INT32;
	m_dataTypeMap["int64"] = DBDataType::INT64;
	m_dataTypeMap["double"] = DBDataType::DOUBLE;
	m_dataTypeMap["varchar"] = DBDataType::STRING;
	m_dataTypeMap["datetime"] = DBDataType::DATETIME;
	m_dataTypeMap["date"] = DBDataType::DATE;
}

void MetaConfig::clean() {
	for (auto& kv : m_tableMap) {
		delete kv.second;
	}
	m_tableMap.clear();
}

void MetaConfig::addTable(const std::string& name, TableInfo* pTable) {
	if (getTableInfo(name) != nullptr) {
		throw new ConfigException("table %s already defined!", name.c_str());
	}
	pTable->setName(name);
	m_tableMap[name] = pTable;
	LOG(INFO, "add table %s", name.c_str());
}

void MetaConfig::load(const std::string& sPath) {
	try {
		std::string line;

		std::ifstream infile(sPath);
		if(infile.fail()){
			throw new ConfigException("reading config file %s failed", sPath.c_str());
		}
		clean();
		TableInfo* pCurrentTable = nullptr;
		while (std::getline(infile, line)) {
			size_t pos = line.find("#");
			if (pos != std::string::npos) {
				line = line.substr(0, pos);
			}
			if (line.find("[table]") != std::string::npos) {
				pCurrentTable = new TableInfo();
				continue;
			}
			std::regex rgx(R"(([^=\s]+)\s*=\s*([^=\s]+))");
			std::smatch matches;
			if (std::regex_search(line, matches, rgx)) {
				if (pCurrentTable != nullptr && matches.size() == 3) {
					std::string sKey = matches[1];
					std::string sValue = matches[2];
					if (sKey == "name") {
						addTable(sValue, pCurrentTable);
					} else if (sKey == "column") {
						pCurrentTable->addColumn(this, sValue);
					} else {
						LOG(INFO, "add attribute %s=%s to table %s",
								sKey.c_str(), sValue.c_str(), pCurrentTable->getName().c_str());
						pCurrentTable->addAttribute(sKey, sValue);
					}
				}
			}
		}

	} catch (const std::ifstream::failure& e) {
		throw new ConfigException("reading config file failure %s", e.what());
	}
}

