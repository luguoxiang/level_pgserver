#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"
#include "execution/DBDataTypeHandler.h"

void ReadFilePlan::explain(std::vector<std::string>& rows) {
	rows.push_back(
			ConcateToString("ReadFile ", m_sPath, ", separator ",
					m_separator));
}

int ReadFilePlan::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < m_columns.size(); ++i) {
		if (m_columns[i]->m_name == pNode->m_sValue) {
			return i;
		}
	}
	return -1;
}

void ReadFilePlan::begin() {
	m_pFile.reset(new std::ifstream(m_sPath));
	if (m_pFile->fail()) {
		EXECUTION_ERROR("File ", m_sPath, " does not exists!");
	}
	m_result.resize(m_columns.size(), ExecutionResult{});
	if (m_ignoreFirstLine) {
		std::getline(*m_pFile, m_line);
	}
}

namespace {
	enum class ParseState {
		OutOfQuote,
		BeginQuote,
		InQuote,
	};
}

void ReadFilePlan::setToken(size_t index, std::string_view token) {
	if(index >= m_columns.size()) {
		return;
	}
	auto pColumn = m_columns[index];
	if(pColumn->m_type == DBDataType::STRING
			&& pColumn->m_iLen > 0
			&& token.length() > pColumn->m_iLen)  {
		EXECUTION_ERROR("Column ", pColumn->m_name, " value exceed defined length ", pColumn->m_iLen);
	}

	DBDataTypeHandler::getHandler(pColumn->m_type)->fromString(token, m_result[index]);
}

bool ReadFilePlan::next() {
	LeafPlan::next();
	if (!std::getline(*m_pFile, m_line)) {
		return false;
	}
	std::string_view line = m_line;

	auto state = ParseState::OutOfQuote;
	size_t targetIndex = 0;
	size_t beginToken = 0;
	size_t tokenIndex = 0;
	for(size_t i=0;i<line.length() && tokenIndex < m_columns.size();++i) {
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

	for (; tokenIndex < m_columns.size(); ++tokenIndex ) {
		setToken(tokenIndex, std::string_view());
	}
	++m_iRowCount;
	return true;

}

void ReadFilePlan::end() {
	m_pFile.reset(nullptr);
}

