#include "PgDataReader.h"
#include "IOException.h"
#include <cassert>
#include "common/MetaConfig.h"
#include "common/ParseTools.h"

PgDataReader::PgDataReader(MemBuffer* pBuffer, size_t len)
:  m_buffer(reinterpret_cast<char*>(pBuffer->data()), len) {
}


std::string_view PgDataReader::getNextString() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	size_t len = strlen(pszCurrent);

	std::string_view s(pszCurrent, len);
	m_iCurrent += len + 1;
	return s;
}

std::string_view PgDataReader::getNextStringWithLen() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int32_t len = ntohl(*(int32_t*) pszCurrent);
	m_iCurrent += 4;
	if (len < 0)
		IO_ERROR("null string is not supported");

	m_iCurrent += len;
	return std::string_view(pszCurrent + 4, len);
}

std::string_view PgDataReader::getNextStringWithShortLen() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int16_t len = ntohs(*(int16_t*) pszCurrent);
	m_iCurrent += 2;
	if (len < 0)
		IO_ERROR("null string is not supported");

	m_iCurrent += len;
	return std::string_view(pszCurrent + 2, len);
}

int16_t PgDataReader::getNextShort() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int16_t sRet =ntohs(*(int16_t*) pszCurrent) ;
	m_iCurrent += 2;
	return sRet;
}

int8_t PgDataReader::getNextByte() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	char c = *pszCurrent;
	m_iCurrent += 1;
	return c;
}

int32_t PgDataReader::getNextInt() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_buffer.size());

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int32_t iRet =ntohl(*(int32_t*) pszCurrent);
	m_iCurrent += 4;
	return iRet;
}


