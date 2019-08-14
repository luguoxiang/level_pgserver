#pragma once
#include <string>
#include <cassert>

class DataReceiver {
public:
	DataReceiver(int fd, bool bNetNumber);
	virtual ~DataReceiver();

	int8_t getNextByte();
	int16_t getNextShort();
	std::string getNextString();
	std::string getNextStringWithLen();
	std::string getNextStringWithShortLen();
	int32_t getNextInt();
	int64_t getNextLongInt();

	size_t getDataLen() {
		return m_nBufLen;
	}

	int getFd() {
		return m_nFd;
	}

	bool hasData() {
		return m_pszCurrent < m_pszBuffer + m_nBufLen;
	}

	void mark() {
		m_pszMark = m_pszCurrent;
	}
	void restore() {
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
