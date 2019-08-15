#include "ParseException.h"
#include <glog/logging.h>

ParseException::ParseException(const std::string& msg) : m_sErrorMsg(msg) {
	LOG(ERROR) << msg;
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
