#include "MetaConfig.h"
#include "common/ConfigException.h"
#include <fstream>
#include <string>
#include <regex>
#include <cassert>
#include <glog/logging.h>

MetaConfig::MetaConfig()  {
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
		std::ostringstream os;
		os <<"table "<<name<<" already defined!";
		throw new ConfigException(os.str());
	}
	pTable->setName(name);
	m_tableMap[name] = pTable;
	LOG(INFO)<< "add table " << name;
}

void MetaConfig::load(const std::string& sPath) {
	try {
		std::string line;

		std::ifstream infile(sPath);
		if(infile.fail()){
			std::ostringstream os;
			os <<"reading config file "<<sPath<<" failed";
			throw new ConfigException(os.str());
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
						LOG(INFO) <<"add attribute %s=%s to table "<< sKey << " = " << sValue << " to table " << pCurrentTable->getName();
						pCurrentTable->addAttribute(sKey, sValue);
					}
				}
			}
		}

	} catch (const std::ifstream::failure& e) {
		std::ostringstream os;
		os <<"reading config file failure: "<<e.what();
		throw new ConfigException(os.str());
	}
}

