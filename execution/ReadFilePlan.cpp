#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"

void ReadFilePlan::explain(std::vector<std::string>& rows) {
	std::ostringstream os;
	os << "ReadFile " << m_sPath << ", seperator " << m_seperator[0];
	rows.push_back(os.str());
}


int ReadFilePlan::addProjection(ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	 for (size_t i = 0; i < m_columns.size(); ++i) {
		if (m_columns[i]->m_sName == pNode->m_sValue) {
			return i;
		}
	}
	return -1;
}

void ReadFilePlan::begin() {
	m_bCancel = false;
	m_pFile.reset(new std::ifstream(m_sPath));
	if (m_pFile->fail()) {
		std::ostringstream os;
		os << "File " << m_sPath << " does not exists!";
		throw new ExecutionException(os.str());
	}
}

bool ReadFilePlan::next() {
	if (m_bCancel)
		return false;

	std::string line;
	if (!std::getline(*m_pFile, line)) {
		return false;
	}
	auto start = 0U;
	auto end = line.find(m_seperator);
	for (int i = 0; i < m_columns.size(); ++i) {
		if(start >= line.length()) {
			m_result[i].m_bNull = true;
			continue;
		}
		std::string token;
		if (end == std::string::npos) {
			token = line.substr(start);
			start = line.length();
		} else {
			token = line.substr(start, end - start);
			start = end + m_seperator.length();
			end = line.find(m_seperator, start);
		}
		m_result[i].m_bNull = false;

		switch (m_columns[i]->m_type) {
		case DBDataType::INT8:
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:
			m_result[i].m_lResult = atoll(token.c_str());
			break;
		case DBDataType::STRING:
			m_result[i].m_sResult = token;
			break;
		case DBDataType::DATETIME:
		case DBDataType::DATE: {
			int64_t iValue = parseTime(token.c_str());
			if (iValue == 0) {
				std::ostringstream os;
				os << "Wrong Time Format:" << token;
				throw new ExecutionException(os.str());
			}
			struct timeval time;
			time.tv_sec = (iValue / 1000000);
			time.tv_usec = (iValue % 1000000);
			m_result[i].m_time = time;
			break;
		}
		case DBDataType::DOUBLE:
			m_result[i].m_dResult = atof(token.c_str());
			break;
		default:
			break;
		}
	}

	++m_iRowCount;
	return true;

}

void ReadFilePlan::end() {
	m_pFile.reset(nullptr);
}

