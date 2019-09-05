#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"
#include "execution/DBDataTypeHandler.h"

void ReadFilePlan::explain(std::vector<std::string>& rows, size_t depth) {
	std::ostringstream os;
	os << std::string(depth, '\t');
	os << "ReadFile "<< m_sPath<< ", separator"<< m_separator;
	rows.push_back(os.str());
}

int ReadFilePlan::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < m_columns.size(); ++i) {
		if (m_columns[i]->m_name == pNode->getString()) {
			return i;
		}
	}
	return -1;
}

void ReadFilePlan::begin() {
	m_file.emplace(m_sPath);
	if (m_file->fail()) {
		EXECUTION_ERROR("File ", m_sPath, " does not exists!");
	}
	m_result.resize(m_columns.size(), ExecutionResult{});
	if (m_ignoreFirstLine) {
		std::getline(*m_file, m_line);
	}
}

namespace {
	enum class ParseState {
		OutOfQuote,
		BeginQuote,
		InQuote,
	};
}

void ReadFilePlan::setToken(size_t index, const char* pszToken, size_t len) {
	if(index >= m_columns.size()) {
		return;
	}
	auto pColumn = m_columns[index];
	if(pColumn->m_type == DBDataType::STRING
			&& pColumn->m_iLen > 0
			&& len > pColumn->m_iLen)  {
		EXECUTION_ERROR("Column ", pColumn->m_name, " value exceed defined length ", pColumn->m_iLen);
	}

	DBDataTypeHandler::getHandler(pColumn->m_type)->fromString(pszToken, len, m_result[index]);
}

bool ReadFilePlan::next() {
	if (!std::getline(*m_file, m_line)) {
		return false;
	}
	checkCancellation();
	char* pszStart = m_line.data();

	auto state = ParseState::OutOfQuote;
	size_t targetIndex = 0;
	size_t beginToken = 0;
	size_t tokenIndex = 0;
	for(size_t i=0; pszStart[i] != '\0' && tokenIndex < m_columns.size();++i) {
		const char c = pszStart[i];
		switch(state){
		case ParseState::OutOfQuote:
			if(c == '"') {
				state = ParseState::BeginQuote;
				continue;
			} else if(c == m_separator) {
				pszStart[targetIndex] = '\0';
				setToken(tokenIndex++, pszStart + beginToken, targetIndex - beginToken);
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

		m_line[targetIndex++] = c;
	}
	pszStart[targetIndex] = '\0';
	setToken(tokenIndex++, pszStart + beginToken, targetIndex - beginToken);

	for (; tokenIndex < m_columns.size(); ++tokenIndex ) {
		setToken(tokenIndex, "", 0);
	}
	++m_iRowCount;
	return true;

}

void ReadFilePlan::end() {
	m_file.reset();
}

