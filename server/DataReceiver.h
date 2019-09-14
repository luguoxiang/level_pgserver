#pragma once
#include <string>
#include <cassert>

class DataReceiver {
public:
	DataReceiver(std::string& buffer);

	int8_t getNextByte();
	int16_t getNextShort();

	//Warning! The return value will be invalid after next getXX call
	std::string_view getNextString();
	std::string_view getNextStringWithLen();
	std::string_view getNextStringWithShortLen();
	int32_t getNextInt();

	size_t getDataLen() {
		return m_iBufLen;
	}


	bool hasData() {
		return m_iCurrent < m_iBufLen;
	}

	void mark() {
		m_iMark = m_iCurrent;
	}
	void restore() {
		m_iCurrent = m_iMark;
	}

	size_t readData(int fd);

private:
	std::string& m_buffer;
	size_t m_iCurrent = -1;
	size_t m_iMark = 0;
	size_t m_iBufLen = 0;
};
