#pragma once

#include <string>
#include <cassert>
#include "IOException.h"
#include "common/ParseNode.h"

class DataSender {
public:
	DataSender(std::string& buffer);

	void addDateTimeAsString(struct tm* pTime, const char* pszFormat, size_t len);

	void addBytesString(const std::string_view s);
	void addString(const std::string_view s);

	void flush(int fd);

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

	bool isBufferFull() {
		return m_bBufferFull;
	}

	void clear() {
		m_bBufferFull = false;
		m_iLastPrepare = m_iWritten = 0;
	}
	template <typename T>
	bool addValueAsString(T value, const char* pszFormat) {
		if(!check(4) ) {
			return false;
		}
		auto iValueStart = m_iWritten + 4;
		auto iAvailable = m_buffer.size() - iValueStart;
		auto iWritten = snprintf(m_buffer.data() + iValueStart, iAvailable, pszFormat, value);

		if (iWritten > 0 && iWritten < iAvailable && addInt32(iWritten)) {
			m_iWritten += iWritten;
			return true;
		} else {
			m_bBufferFull = true;
			return false;
		}
	}

private:
	bool addStringZeroEnd(const std::string_view s);
	bool addInt32(int32_t value);

	bool check(uint32_t iSize) {
		if(m_bBufferFull) {
			return false;
		}

		if(m_iWritten + iSize <= m_buffer.size()) {
			return true;
		} else{
			m_bBufferFull = true;
			return false;
		}
	}

	std::string& m_buffer;
	uint32_t m_iWritten;
	uint32_t m_iLastPrepare;
	bool m_bBufferFull = false;
};
