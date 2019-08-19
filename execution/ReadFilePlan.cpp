#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"

void ReadFilePlan::explain(std::vector<std::string>& rows) {
	rows.push_back(
			ConcateToString("ReadFile ", m_sPath, ", seperator ",
					m_seperator[0]));
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
		throw new ExecutionException(
				ConcateToString("File ", m_sPath, " does not exists!"));
	}
}

bool ReadFilePlan::next() {
	if (m_bCancel)
		return false;

	if (!std::getline(*m_pFile, m_line)) {
		return false;
	}
	std::string_view line = m_line;

	auto start = 0U;
	auto end = line.find(m_seperator);
	for (int i = 0; i < m_columns.size(); ++i) {
		if (start >= line.length()) {
			m_result[i].setNull();
			continue;
		}
		std::string_view token;
		if (end == std::string::npos) {
			token = line.substr(start);
			start = line.length();
		} else {
			token = line.substr(start, end - start);
			start = end + m_seperator.length();
			end = line.find(m_seperator, start);
		}

		switch (m_columns[i]->m_type) {
		case DBDataType::INT8:
		case DBDataType::INT16:
		case DBDataType::INT32:
		case DBDataType::INT64:

			m_result[i].setInt(Tools::toInt(token));
			break;
		case DBDataType::STRING:
			m_result[i].setStringView(token);
			break;
		case DBDataType::DATETIME:
		case DBDataType::DATE:
			if (int64_t iValue = parseTime(
					std::string(token.data(), token.length()).c_str()); iValue
					> 0) {
				struct timeval time;
				time.tv_sec = (iValue / 1000000);
				time.tv_usec = (iValue % 1000000);
				m_result[i].setTime(time);
				break;
			} else {
				throw new ExecutionException(
						ConcateToString("Wrong Time Format:", token));
			}
		case DBDataType::DOUBLE:

			try {
				std::string s(token.data(), token.length());
				m_result[i].setDouble(std::stof(s));
			} catch (const std::exception& e) {
				throw new ExecutionException(e.what());
			}
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

