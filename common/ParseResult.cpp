#include "ParseResult.h"
#include "QueryRewritter.h"

ParseResult::ParseResult() {
	m_pBuffer = GlobalMemBlockPool::getInstance().alloc();
	if(!m_pBuffer) {
		PARSE_ERROR("Not enough parse buffer blocks");
	}
	if (parseInit(this)) {
		PARSE_ERROR("Failed to init parser!");
	}
}

ParseResult::~ParseResult()  {
	parseTerminate(this);
	GlobalMemBlockPool::getInstance().free(std::move(m_pBuffer));
}

char* ParseResult::alloc(size_t size)  {
	if (m_iCurrent + size > GlobalMemBlockPool::BLOCK_SIZE) {
		PARSE_ERROR("Not enough parse buffer");
	}
	char* alloc = reinterpret_cast<char*>(m_pBuffer->data() + m_iCurrent);
	m_iCurrent += size;
	return alloc;
}

std::string_view ParseResult::allocString(std::string_view s, bool zeroEnded) {
	size_t len = s.length();

	if (zeroEnded) {
		char* pszAlloc = alloc(len + 1);
		std::copy(s.data(), s.data() + len, pszAlloc);
		pszAlloc[len] = '\0';
		return std::string_view(pszAlloc, len);
	} else {
		char *pszAlloc = alloc(len);
		std::copy(s.data(), s.data() + len, pszAlloc);
		return std::string_view(pszAlloc, len);
	}
}

void ParseResult::initParse(const std::string_view sql) {
	m_yycolumn = 1;
	m_yylineno = 1;
	m_iCurrent = 0;
	m_pResult = nullptr;
	m_bindParamNodes.clear();
	m_sSql = allocString(sql, true);
	m_sError = "";
}

void ParseResult::parse(const std::string_view sql) {
	parseSql(this, sql);
	if (m_pResult == nullptr) {
		throw ParseException(*this);
	}

	QueryRewritter rewritter(*this);
	m_pResult = rewritter.rewrite(m_pResult);
#ifndef NDEBUG
	m_pResult->print(0);
#endif
}
