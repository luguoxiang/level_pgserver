#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <cassert>
#include <stdio.h>
#include "DataSender.h"

DataSender::DataSender(int fd,  uint32_t iSendBuffer) :
		m_nFd(fd), m_iWritten(0), m_iLastPrepare(0),  m_buffer(iSendBuffer, ' ') {
}

DataSender::~DataSender() {
}

void DataSender::addFloat(float value) {
	auto p = reinterpret_cast<int32_t*>(&value);
	int32_t netval = htonl(*p);

	check(4);
	m_buffer.replace(m_iWritten, 4, reinterpret_cast<const char*>(&netval), 4);
	m_iWritten += 4;
}
void DataSender::addDouble(double value) {
	uint64_t iValue = static_cast<uint64_t>(value);
	int32_t low = static_cast<int32_t>(iValue & 0xffffffff);
	int32_t hight = iValue >> 32;
	addInt(hight);
	addInt(low);
}

void DataSender::directSend(const std::string_view s) {
	if (write(m_nFd, s.data(), s.size()) != 1)
			{
		IO_ERROR("send() failed!");
	}
}


void DataSender::setInt(size_t iOffset, int32_t value) {
	if (iOffset + m_iLastPrepare + 4 > m_iWritten) {
		IO_ERROR("write overflow for DataSender!");
	}
	int32_t netval = htonl(value);

	m_buffer.replace(m_iLastPrepare + iOffset, 4, reinterpret_cast<const char*>(&netval), 4);
}

void DataSender::begin() {
	m_iLastPrepare = m_iWritten;
}

void DataSender::addByte(int8_t value) {
	check(1);
	m_buffer[m_iWritten] = value;
	++m_iWritten;
}

void DataSender::addInt(int32_t value) {
	int32_t netval = htonl(value);

	check(4);
	m_buffer.replace(m_iWritten, 4, reinterpret_cast<const char*>(&netval), 4);
	m_iWritten += 4;
}

void DataSender::addInt64(int64_t value) {
	int32_t low = static_cast<int32_t>(value & 0xffffffff);
	int32_t hight = value >> 32;
	addInt(hight);
	addInt(low);
}

void DataSender::addShort(int16_t value) {
	int16_t netval = htons(value);

	check(2);
	m_buffer.replace(m_iWritten, 2, reinterpret_cast<const char*>(&netval), 2);
	m_iWritten += 2;
}

void DataSender::addStringZeroEnd(const std::string_view s) {
	auto len = s.length();
	check( len + 1);
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
	m_buffer[m_iWritten] = '\0';
	++m_iWritten;
}

constexpr auto DIGITS = "0123456789ABCDEF";

void DataSender::addBytesString(const std::string_view s) {
	auto len = 2 + s.length() * 2;
	addInt(len);
	check(len);
	m_buffer[m_iWritten++] = '\\';
	m_buffer[m_iWritten++] = 'x';

	for (size_t i = 0; i   <  s.length() ; ++i) {
		uint8_t c = s[i];

		m_buffer[m_iWritten++] = DIGITS[c >> 4];
		m_buffer[m_iWritten++] = DIGITS[c & 0xf];
	}
}

void DataSender::addString(const std::string_view s) {
	auto len = s.length();
	addInt(len);
	check(len);
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
}

void DataSender::addDateTimeAsString(struct tm* pTime, const char* pszFormat, size_t len) {
	addInt(len);
	check(len+1);
	auto iWritten = strftime(m_buffer.data() + m_iWritten, m_buffer.size() - m_iWritten, pszFormat, pTime);
	assert(iWritten == len);
	m_iWritten += iWritten;
}

void DataSender::end() {
	m_iLastPrepare = m_iWritten;
}

void DataSender::flush() {
	if (m_iLastPrepare == 0)
		return;

	assert(m_iWritten >= m_iLastPrepare);

	//Because we must write back package length at m_iLastPrepare.
	//We could not send data after m_iLastPrepare.
	uint32_t nWrite = send(m_nFd, m_buffer.data(), m_iLastPrepare, 0);
	if (nWrite != m_iLastPrepare) {
		IO_ERROR("Could not send data\n");
	}

	m_iWritten -= m_iLastPrepare;

	memcpy(m_buffer.data(), m_buffer.data() + m_iLastPrepare, m_iWritten);
	m_iLastPrepare = 0;
}
