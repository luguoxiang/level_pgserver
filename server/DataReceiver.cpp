#include "DataReceiver.h"
#include "IOException.h"
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include "common/MetaConfig.h"

DataReceiver::DataReceiver(std::string& buffer)
:  m_buffer(buffer) {
}


std::string_view DataReceiver::getNextString() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	size_t len = strlen(pszCurrent);

	std::string_view s(pszCurrent, len);
	m_iCurrent += len + 1;
	return s;
}

std::string_view DataReceiver::getNextStringWithLen() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int32_t len = ntohl(*(int32_t*) pszCurrent);
	m_iCurrent += 4;
	if (len <= 0)
		IO_ERROR("null string is not supported");

	m_iCurrent += len;
	return std::string_view(pszCurrent + 4, len);
}

std::string_view DataReceiver::getNextStringWithShortLen() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int16_t len = ntohs(*(int16_t*) pszCurrent);
	m_iCurrent += 2;
	if (len <= 0)
		IO_ERROR("null string is not supported");

	m_iCurrent += len;
	return std::string_view(pszCurrent + 2, len);
}

int16_t DataReceiver::getNextShort() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int16_t sRet =ntohs(*(int16_t*) pszCurrent) ;
	m_iCurrent += 2;
	return sRet;
}

int8_t DataReceiver::getNextByte() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	char c = *pszCurrent;
	m_iCurrent += 1;
	return c;
}

int32_t DataReceiver::getNextInt() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int32_t iRet =ntohl(*(int32_t*) pszCurrent);
	m_iCurrent += 4;
	return iRet;
}

size_t DataReceiver::readData(int fd) {
	if (::recv(fd, (char*) &m_iBufLen, 4, 0) != 4) {
		IO_ERROR("Unexpect EOF!");
	}
	m_iBufLen = ntohl(m_iBufLen);
	if (m_iBufLen < 4) {
		IO_ERROR("Invalid message length!");
	}
	m_iBufLen -= 4;

	if(m_buffer.size() < m_iBufLen + 1) {
		IO_ERROR("Not enough receive buffer");
	}

	size_t readCount = 0;
	while (m_iBufLen > readCount) {
		int count = ::read(fd, m_buffer.data() + readCount, m_iBufLen - readCount);
		if (count < 0) {
			IO_ERROR("read() failed!");
		}
		readCount += count;
	}

	m_buffer[m_iBufLen] = 0;
	m_iCurrent = 0;
	return readCount;
}
