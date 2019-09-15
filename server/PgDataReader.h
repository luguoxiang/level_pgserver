#pragma once
#include <string>
#include <cassert>

class PgDataReader {
public:
	PgDataReader(std::string_view buffer);

	int8_t getNextByte();
	int16_t getNextShort();

	//Warning! The return value will be invalid after next getXX call
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
