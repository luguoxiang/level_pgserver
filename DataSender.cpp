#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <cassert>

#include "DataSender.h"

DataSender::DataSender(int fd, bool bNetNumber, uint32_t iSendBuffer) :
		m_nFd(fd), m_iWritten(0), m_iLastPrepare(0), m_bNetNumber(bNetNumber), m_buffer(iSendBuffer, ' ') {
}

DataSender::~DataSender() {
}

void DataSender::setInt(size_t iOffset, int32_t value) {
	if (iOffset + m_iLastPrepare + 4 > m_iWritten) {
		throw new IOException("write overflow for DataSender!");
	}
	int32_t netval = m_bNetNumber ? htonl(value) : value;

	memcpy(m_buffer.data() + m_iLastPrepare + iOffset, &netval, 4);
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
	int32_t netval = m_bNetNumber ? htonl(value) : value;

	check(4);
	m_buffer.replace(m_iWritten, 4, reinterpret_cast<const char*>(&netval), 4);
	m_iWritten += 4;
}

void DataSender::addLongInt(int64_t value) {
	if (m_bNetNumber) {
		throw new IOException("hton is not supported for long int!");
	}
	check(8);
	m_buffer.replace(m_iWritten, 8, reinterpret_cast<const char*>(&value), 8);
	m_iWritten += 8;
}

void DataSender::addShort(int16_t value) {
	int16_t netval = m_bNetNumber ? htons(value) : value;

	check(2);
	m_buffer.replace(m_iWritten, 2, reinterpret_cast<const char*>(&netval), 2);
	m_iWritten += 2;
}

void DataSender::addString(const std::string_view s) {
	auto len = s.length();
	check( len + 1);
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
	m_buffer[m_iWritten] = '\0';
	++m_iWritten;
}
void DataSender::addStringAndLength(const std::string_view s) {
	auto len = s.length();
	addInt(len);
	check(len);
	m_buffer.replace(m_iWritten, len, s.data(), len);
	m_iWritten += len;
}

void DataSender::addChar(char c) {
	m_buffer[m_iWritten] = c;
	++m_iWritten;
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
		throw new IOException("Could not send data\n");
	}

	m_iWritten -= m_iLastPrepare;

	memcpy(m_buffer.data(), m_buffer.data() + m_iLastPrepare, m_iWritten);
	m_iLastPrepare = 0;
}
