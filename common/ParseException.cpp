#include "ParseException.h"
#include <stdarg.h>
ParseException::ParseException(const char* pszMsg, ...) :
		m_iStartCol(0), m_iEndCol(0), m_iLine(-1) {
	va_list arg;
	va_start(arg, pszMsg);

	char szBuf[300];
	vsnprintf(szBuf, 300, pszMsg, arg);
	m_sErrorMsg = szBuf;

	va_end(arg);
}

ParseException::ParseException(ParseResult* pResult) :
		m_sErrorMsg(pResult->m_szErrorMsg), m_iStartCol(pResult->m_iStartCol), m_iEndCol(
				pResult->m_iEndCol), m_iLine(pResult->m_iLine) {
}

ParseException::~ParseException() {
}

void ParseException::printLocation() {
	if (m_iLine < 0)
		return;
	int j;
	for (j = 0; j < m_iStartCol; ++j) {
		printf(" ");
	}
	for (j = m_iStartCol; j <= m_iEndCol; ++j) {
		printf("^");
	}
	printf("\n");
}
