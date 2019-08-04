#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

class DataReceiver
{
public:
	DataReceiver(int fd, bool bNetNumber);
	virtual ~DataReceiver();

	const char* getNextString(size_t* pLen);
	int8_t getNextByte();
	int16_t getNextShort();
	const char* getNextStringWithLen(uint32_t* pLen);
	const char* getNextStringWithShortLen(uint16_t* pLen);
	int32_t getNextInt();
	int64_t getNextLongInt();

	size_t getDataLen()
	{
		return m_nBufLen;
	}

	int getFd()
	{
		return m_nFd;
	}

	bool hasData()
	{
		return m_pszCurrent < m_pszBuffer + m_nBufLen;
	}

	void mark()
	{
		m_pszMark = m_pszCurrent;
	}
	void restore()
	{
		assert(m_pszMark);
		m_pszCurrent = m_pszMark;
	}

	size_t readData();
	char readByte();

private:
	char* m_pszBuffer;
	char* m_pszCurrent;
	char* m_pszMark;
	int m_nFd;
	size_t m_nBufLen;
	bool m_bNetNumber;
};
#endif  // DATA_RECEIVER_H
