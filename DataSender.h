#pragma once

#include <string>
#include <cassert>
#include "IOException.h"
#include "common/ParseNode.h"

class DataSender {
public:
	DataSender(int fd, uint32_t iSendBuffer);
	virtual ~DataSender();

	void addFloat(float value);
	void addDouble(double value);

	void addByte(int8_t value);
	void addInt(int32_t value);
	void addShort(int16_t value);
	void addInt64(int64_t value);
	void addStringZeroEnd(const std::string_view s);

	void addIntAsString(int64_t value) {
		addValueAsString(value, "%lld");
	}

	void addDoubleAsString(double value) {
		addValueAsString(value, "%f");
	}

	void addDateAsString(struct tm* pTime);
	void addDateTimeAsString(struct tm* pTime);

	void addBytesString(const std::string_view s);
	void addString(const std::string_view s);


	void flush();

	size_t getWritten() {
		return m_iWritten - m_iLastPrepare;
	}
	//iOffset should lower or equals than end() - begin()
	void setInt(size_t iOffset, int32_t value);

	void directSend(const std::string_view s);

	DataSender& operator <<(const std::string_view s) {
		addStringZeroEnd(s);
		return *this;
	}

	DataSender& operator <<(int32_t t) {
		addInt(t);
		return *this;
	}

	DataSender& operator <<(int16_t value) {
		addShort(value);
		return *this;
	}

	DataSender& operator <<(int8_t value) {
		addByte(value);
		return *this;
	}

	void begin();
	void end();

private:
	void check(uint32_t iSize) {
		if (m_iWritten + iSize > m_buffer.size()) {
			flush();
		}
		if (m_iWritten + iSize > m_buffer.size()) {
			IO_ERROR("Send data is too large:written=",m_iWritten, ", total=", m_buffer.size(), ", require=", iSize);
		}
	}

	template <typename T>
	void addValueAsString(T value, const char* pszFormat) {
		check(4);
		while(true) {
			auto iValueStart = m_iWritten + 4;
			auto iAvailable = m_buffer.size() - iValueStart;
			auto iWritten = snprintf(m_buffer.data() + iValueStart, iAvailable, pszFormat, value);

			if (iWritten > 0 && iWritten < iAvailable) {
				addInt(iWritten);
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


	int m_nFd;
	std::string m_buffer;
	uint32_t m_iWritten;
	uint32_t m_iLastPrepare;
};
