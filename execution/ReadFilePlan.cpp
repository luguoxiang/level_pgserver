#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"

void ReadFilePlan::explain(std::vector<std::string>& rows) {
	rows.push_back(
			ConcateToString("ReadFile ", m_sPath, ", separator ",
					m_separator));
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
	m_result.reserve(m_columns.size());
}

namespace {
	enum class ParseState {
		OutOfQuote,
		BeginQuote,
		InQuote,
	};
}

void ReadFilePlan::setToken(size_t index, std::string_view token) {
	if(index > m_columns.size()) {
		return;
	}
	switch (m_columns[index]->m_type) {
	case DBDataType::INT8:
	case DBDataType::INT16:
	case DBDataType::INT32:
	case DBDataType::INT64:
		m_result[index].setInt(Tools::toInt(token));
		break;
	case DBDataType::STRING:
		m_result[index].setStringView(token);
		break;
	case DBDataType::DATETIME:
	case DBDataType::DATE:
		if (int64_t iValue = parseTime(token); iValue > 0) {
			m_result[index].setInt(iValue);
			break;
		} else {
			throw new ExecutionException(
					ConcateToString("Wrong Time Format:", token));
		}
	case DBDataType::DOUBLE:
		try {
			std::string s(token.data(), token.length());
			m_result[index].setDouble(std::stof(s));
		} catch (const std::exception& e) {
			throw new ExecutionException(e.what());
		}
		break;
	default:
		break;
	}
}

bool ReadFilePlan::next() {
	if (m_bCancel)
		return false;

	if (!std::getline(*m_pFile, m_line)) {
		return false;
	}
	std::string_view line = m_line;

	auto state = ParseState::OutOfQuote;
	size_t targetIndex = 0;
	size_t beginToken = 0;
	size_t tokenIndex = 0;
	for(size_t i=0;i<line.length();++i) {
		char c = line[i];
		switch(state){
		case ParseState::OutOfQuote:
			if(c == '"') {
				state = ParseState::BeginQuote;
				continue;
			} else if(c == m_separator) {
				setToken(tokenIndex++, std::string_view{line.data() + beginToken, targetIndex - beginToken});
				beginToken = targetIndex;
				continue;
			}
			break;
		case ParseState::BeginQuote:
			if (c == '"') {
				state = ParseState::OutOfQuote;
			} else {
				state = ParseState::InQuote;
			}
			break;
		case ParseState::InQuote:
			if (c == '"') {
				state = ParseState::OutOfQuote;
				continue;
			}
			break;
		default:
			assert(0);
			break;
		}

		m_line[targetIndex++] = line[i];
	}
	setToken(tokenIndex++, std::string_view{line.data() + beginToken, targetIndex - beginToken});

	++m_iRowCount;
	return true;

}

void ReadFilePlan::end() {
	m_pFile.reset(nullptr);
}

