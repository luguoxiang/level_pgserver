#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <cassert>
#include <stdio.h>
#include "DataSender.h"
#include "common/MetaConfig.h"

DataSender::DataSender() :
		m_iWritten(0), m_iLastPrepare(0),  m_buffer(MetaConfig::getInstance().getNetworkBuffer(), '\0') {
}

DataSender& DataSender::operator <<(float value) {
	auto p = reinterpret_cast<int32_t*>(&value);
	addInt32(*p);
	return *this;
}

DataSender& DataSender::operator <<(double value) {
	int32_t* iValue = reinterpret_cast<int32_t*>(&value);
	addInt32(iValue[1]) && addInt32(iValue[0]);
	return *this;
}

void DataSender::begin(int8_t cMsgType) {
	m_bBufferFull = false;
	m_iLastPrepare = m_iWritten;
	*this << cMsgType << static_cast<int32_t>(0); //write back later
}

void DataSender::end() {
	if(m_bBufferFull) {
		return;
	}
	assert (m_iLastPrepare + 5 <= m_iWritten);
	int32_t netval = htonl(m_iWritten - m_iLastPrepare - 1);
	m_buffer.replace(m_iLastPrepare + 1, 4, reinterpret_cast<const char*>(&netval), 4);

	m_iLastPrepare = m_iWritten;
}

DataSender& DataSender::operator <<(int8_t value) {
	if(!check(1) ) {
		return *this;
	}
	m_buffer[m_iWritten] = value;
	++m_iWritten;
	return *this;
}

bool DataSender::addInt32(int32_t value) {
	if(!check(4)) {
		return false;
	}

	int32_t netval = htonl(value);
	m_buffer.replace(m_iWritten, 4, reinterpret_cast<const char*>(&netval), 4);
	m_iWritten += 4;
	return true;
}

DataSender& DataSender::operator <<(int64_t value) {
	int32_t* iValue = reinterpret_cast<int32_t*>(&value);
	addInt32(iValue[1]) && 	addInt32(iValue[0]);
	return *this;
}

DataSender& DataSender::operator <<(int16_t value) {
	if(!check(2)) {
		return *this;
	}
	int16_t netval = htons(value);
	m_buffer.replace(m_iWritten, 2, reinterpret_cast<const char*>(&netval), 2);
	m_iWritten += 2;
	return *this;
}

bool DataSender::addStringZeroEnd(const std::string_view s) {
	auto len = s.length();
	if(!check( len + 1) ){
		return false;
	}
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
	m_buffer[m_iWritten] = '\0';
	++m_iWritten;
	return true;
}

constexpr auto DIGITS = "0123456789ABCDEF";

void DataSender::addBytesString(const std::string_view s) {
	auto len = 2 + s.length() * 2;
	if(!addInt32(len) || !check(len)) {
		return;
	}
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
	if(!addInt32(len)  || !check(len) ) {
		return;
	}
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
}

void DataSender::addDateTimeAsString(struct tm* pTime, const char* pszFormat, size_t len) {
	if(!addInt32(len) || !check(len+1) ) {
		return;
	}
	auto iWritten = strftime(m_buffer.data() + m_iWritten, m_buffer.size() - m_iWritten, pszFormat, pTime);
	assert(iWritten == len);
	m_iWritten += iWritten;
}

void DataSender::flush(int fd) {
	if (m_iLastPrepare == 0)
		return;

	assert(m_iWritten >= m_iLastPrepare);

	//Because we must write back package length at m_iLastPrepare.
	//We could not send data after m_iLastPrepare.
	uint32_t nWrite = ::send(fd, m_buffer.data(), m_iLastPrepare, 0);
	if (nWrite != m_iLastPrepare) {
		IO_ERROR("Could not send data\n");
	}

	//discard uncommitted data
	m_iWritten = m_iLastPrepare = 0;
}
