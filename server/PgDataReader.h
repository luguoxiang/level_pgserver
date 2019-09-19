#pragma once
#include <string>
#include <cassert>
#include <vector>
#include <cstddef>

#include "common/GlobalMemBlockPool.h"

class PgDataReader {
public:
	PgDataReader(MemBuffer* pBuffer, size_t len);

	int8_t getNextByte();
	int16_t getNextShort();

	std::string_view getNextString();
	std::string_view getNextStringWithLen();
	std::string_view getNextStringWithShortLen();
	int32_t getNextInt();

	bool hasData() {
		return m_iCurrent < m_buffer.size();
	}

private:
	std::string_view m_buffer;
	size_t m_iCurrent = 0;
};
