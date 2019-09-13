#include "DataReceiver.h"
#include "IOException.h"
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

DataReceiver::DataReceiver(int fd)
		: m_nFd(fd){
}

DataReceiver::~DataReceiver() {
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
		throw new IOException("null string is not supported");

	m_iCurrent += len;
	return std::string_view(pszCurrent + 4, len);
}

std::string_view DataReceiver::getNextStringWithShortLen() {
	assert(m_iCurrent >= 0 && m_iCurrent < m_iBufLen);

	const char* pszCurrent = m_buffer.data() + m_iCurrent;
	int16_t len = ntohs(*(int16_t*) pszCurrent);
	m_iCurrent += 2;
	if (len <= 0)
		throw new IOException("null string is not supported");

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



char DataReceiver::readByte() {
	char qtype;
	int ret = recv(m_nFd, &qtype, 1, 0);

	if (ret != 1) {
		throw new IOException("read() failed!");
	}
	return qtype;
}

size_t DataReceiver::readData() {
	if (recv(m_nFd, (char*) &m_iBufLen, 4, 0) != 4) {
		throw new IOException("Unexpect EOF!");
	}
	m_iBufLen = ntohl(m_iBufLen);
	if (m_iBufLen < 4) {
		throw new IOException("Invalid message length!");
	}
	m_iBufLen -= 4;

	m_buffer.clear();
	m_buffer.reserve(m_iBufLen + 1);

	size_t readCount = 0;
	while (m_iBufLen > readCount) {
		int count = read(m_nFd, m_buffer.data() + readCount, m_iBufLen - readCount);
		if (count < 0) {
			throw new IOException("read() failed!");
		}
		readCount += count;
	}

	m_buffer[m_iBufLen] = 0;
	m_iCurrent = 0;
	return readCount;
}
