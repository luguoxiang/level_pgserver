#include "DataReceiver.h"
#include "common/IOException.h"
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include "common/Log.h"

#define PG_PROTOCOL(m,n)        (((m) << 16) | (n))
#define PG_PROTOCOL_MAJOR(v)    ((v) >> 16)
#define PG_PROTOCOL_MINOR(v)    ((v) & 0x0000ffff)

#define CANCEL_REQUEST_CODE PG_PROTOCOL(1234,5678)
#define NEGOTIATE_SSL_CODE PG_PROTOCOL(1234,5679)

DataReceiver::DataReceiver(int fd, bool bNetNumber)
		: m_pszBuffer(NULL), m_pszCurrent(NULL), m_pszMark(NULL), m_nFd(fd), m_nBufLen(
				0), m_bNetNumber(bNetNumber)
{
}

DataReceiver::~DataReceiver()
{
	if (m_pszBuffer)
		delete[] m_pszBuffer;
}

const char* DataReceiver::getNextString(size_t* pLen)
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + m_nBufLen);
	const char* pszRet = m_pszCurrent;
	*pLen = strlen(m_pszCurrent);
	m_pszCurrent += *pLen + 1;

	return pszRet;
}

const char* DataReceiver::getNextStringWithLen(uint32_t* pLen)
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + m_nBufLen);
	*pLen = m_bNetNumber ?
			htonl(*(uint32_t*) m_pszCurrent) : *(uint32_t*) m_pszCurrent;
	m_pszCurrent += 4;
	const char* pszRet = m_pszCurrent;

	if (*pLen <= 0)
		return NULL;

	m_pszCurrent += *pLen;
	return pszRet;
}
const char* DataReceiver::getNextStringWithShortLen(uint16_t* pLen)
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + m_nBufLen);
	*pLen = m_bNetNumber ?
			htons(*(uint16_t*) m_pszCurrent) : *(uint16_t*) m_pszCurrent;
	m_pszCurrent += 2;
	const char* pszRet = m_pszCurrent;

	if (*pLen <= 0)
		return NULL;

	m_pszCurrent += *pLen;
	return pszRet;
}

int16_t DataReceiver::getNextShort()
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + m_nBufLen);
	int16_t sRet =
			m_bNetNumber ?
					htons(*(int16_t*) m_pszCurrent) : *(int16_t*) m_pszCurrent;
	m_pszCurrent += 2;
	return sRet;
}

int8_t DataReceiver::getNextByte()
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + 1);
	char c = *m_pszCurrent;
	m_pszCurrent += 1;
	return c;
}

int32_t DataReceiver::getNextInt()
{
	assert(m_pszCurrent);
	assert(m_pszCurrent < m_pszBuffer + m_nBufLen);
	int32_t iRet =
			m_bNetNumber ?
					htonl(*(int32_t*) m_pszCurrent) : *(int32_t*) m_pszCurrent;
	m_pszCurrent += 4;
	return iRet;
}

int64_t DataReceiver::getNextLongInt()
{
	assert(m_pszCurrent);
	if (m_pszCurrent >= m_pszBuffer + m_nBufLen)
	{
		throw new IOException("Illegal receive data!");
	}
	if (m_bNetNumber)
	{
		throw new IOException("hton is not supported for long int");
	}

	int64_t iRet = *(int64_t*) m_pszCurrent;
	m_pszCurrent += 8;
	return iRet;
}

char DataReceiver::readByte()
{
	char qtype;
	int ret = recv(m_nFd, &qtype, 1, 0);

	if (ret != 1)
	{
		throw new IOException("read() failed!");
	}
	return qtype;
}

size_t DataReceiver::readData()
{
	if (recv(m_nFd, (char*) &m_nBufLen, 4, 0) != 4)
	{
		throw new IOException("Unexpect EOF!");
	}
	m_nBufLen = m_bNetNumber ? ntohl(m_nBufLen) : m_nBufLen;
	if (m_nBufLen < 4)
	{
		throw new IOException("Invalid message length!");
	}
	m_nBufLen -= 4;

	if (m_pszBuffer)
		delete[] m_pszBuffer;
	m_pszBuffer = new char[m_nBufLen + 1];

	size_t readCount = 0;
	while (m_nBufLen > readCount)
	{
		int count = read(m_nFd, m_pszBuffer + readCount, m_nBufLen - readCount);
		if (count < 0)
		{
			throw new IOException("read() failed!");
		}
		readCount += count;
	}

	m_pszBuffer[m_nBufLen] = 0;
	m_pszCurrent = m_pszBuffer;
	return readCount;
}