#pragma once

#include <string>
#include "IOException.h"

class DataSender {
public:
	DataSender(int fd, bool bNetNumber, uint32_t iSendBuffer);
	virtual ~DataSender();

	void addByte(int8_t value);
	void addInt(int32_t value);
	void addShort(int16_t value);
	void addLongInt(int64_t value);
	void addString(const std::string_view s);
	void addStringAndLength(const std::string_view s);
	void addChar(char c);

	void flush();

	size_t getWritten() {
		return m_iWritten - m_iLastPrepare;
	}
	//iOffset should lower or equals than end() - begin()
	void setInt(size_t iOffset, int32_t value);

protected:
	void begin();
	void end();

private:
	void check(uint32_t iSize) {
		if (m_iWritten + iSize > m_iSendBuffer) {
			flush();
		}
		if (m_iWritten + iSize > m_iSendBuffer) {
			char szBuf[1024];
			snprintf(szBuf, 1024,
					"Send data is too large:written=%d, total=%d, require=%d!",
					m_iWritten, m_iSendBuffer, iSize);
			throw new IOException(szBuf);
		}
	}

	int m_nFd;
	char* m_szBuffer;
	uint32_t m_iWritten;
	uint32_t m_iLastPrepare;
	bool m_bNetNumber;

	uint32_t m_iSendBuffer;
};
