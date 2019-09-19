#include "ParseException.h"
#include "ParseResult.h"
#include <glog/logging.h>

ParseException::ParseException(const std::string& msg) : m_sErrorMsg(msg) {

}

ParseException::ParseException(ParseResult& pr)
	: m_sErrorMsg(pr.m_sError)
	, m_iStartCol(pr.m_iStartCol)
	, m_iEndCol(pr.m_iEndCol)
	, m_iLine(pr.m_iLine) {
	LOG(ERROR) << m_sErrorMsg;
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
