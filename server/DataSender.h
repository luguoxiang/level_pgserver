#pragma once

#include <string>
#include <cassert>
#include "IOException.h"
#include "common/ParseNode.h"

class DataSender {
public:
	DataSender(int fd, uint32_t iSendBuffer);
	virtual ~DataSender();

	void addDateTimeAsString(struct tm* pTime, const char* pszFormat, size_t len);

	void addBytesString(const std::string_view s);
	void addString(const std::string_view s);

	void flush();

	void directSend(const std::string_view s);

	DataSender& operator <<(nullptr_t) {
		addInt32(-1);
		return *this;
	}

	DataSender& operator <<(const std::string_view s) {
		addStringZeroEnd(s);
		return *this;
	}

	DataSender& operator <<(float value);
	DataSender& operator <<(double value);

	DataSender& operator <<(int64_t t);

	DataSender& operator <<(int32_t t) {
		addInt32(t);
		return *this;
	}

	DataSender& operator <<(int16_t value);

	DataSender& operator <<(int8_t value);

	void begin(int8_t cMsgType);
	void end();

	template <typename T>
	void addValueAsString(T value, const char* pszFormat) {
		check(4);
		while(true) {
			auto iValueStart = m_iWritten + 4;
			auto iAvailable = m_buffer.size() - iValueStart;
			auto iWritten = snprintf(m_buffer.data() + iValueStart, iAvailable, pszFormat, value);

			if (iWritten > 0 && iWritten < iAvailable) {
				addInt32(iWritten);
				m_iWritten += iWritten;
				break;
			} else if(iWritten > 0) {
				//not enough buffer, required iWritten + 4(length)
				check(iWritten + 4);
			} else {
				assert(0);
			}
		}
	}

private:
	void addStringZeroEnd(const std::string_view s);
	void addInt32(int32_t value);

	void check(uint32_t iSize) {
		if (m_iWritten + iSize > m_buffer.size()) {
			flush();
		}
		if (m_iWritten + iSize > m_buffer.size()) {
			IO_ERROR("Send data is too large:written=",m_iWritten, ", total=", m_buffer.size(), ", require=", iSize);
		}
	}

	int m_nFd;
	std::string m_buffer;
	uint32_t m_iWritten;
	uint32_t m_iLastPrepare;
};
