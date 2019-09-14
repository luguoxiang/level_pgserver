#include "ParseResult.h"

constexpr int PARSE_BUFFER_SIZE = 512 * 1024;

ParseResult::ParseResult() : m_sParseBuffer(PARSE_BUFFER_SIZE, '\0') {
}

char* ParseResult::alloc(size_t size)  {
	if (m_iCurrent + size > PARSE_BUFFER_SIZE) {
		return nullptr;
	}
	char* alloc = m_sParseBuffer.data() + m_iCurrent;
	m_iCurrent += size;
	return alloc;
}

void ParseResult::initParse(const std::string_view sql) {
	m_yycolumn = 1;
	m_yylineno = 1;
	m_iCurrent = 0;
	m_pResult = nullptr;
	m_bindParamNodes.clear();
	m_sSql.assign(sql.data(), sql.length());
	m_sError = "";
}
