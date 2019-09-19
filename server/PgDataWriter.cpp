#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <cassert>
#include <stdio.h>
#include "PgDataWriter.h"
#include "common/MetaConfig.h"

PgDataWriter::PgDataWriter(MemBuffer* pData) : m_pData(pData) {
}

PgDataWriter& PgDataWriter::operator <<(float value) {
	auto p = reinterpret_cast<int32_t*>(&value);
	addInt32(*p);
	return *this;
}

PgDataWriter& PgDataWriter::operator <<(double value) {
	int32_t* iValue = reinterpret_cast<int32_t*>(&value);
	addInt32(iValue[1]) && addInt32(iValue[0]);
	return *this;
}

void PgDataWriter::begin(int8_t cMsgType) {
	m_bBufferFull = false;
	*this << cMsgType << static_cast<int32_t>(0); //write back later
}

void PgDataWriter::end() {
	if(m_bBufferFull) {
		return;
	}
	assert (m_iLastPrepare + 5 <= m_iWritten);

	int32_t* pTarget = reinterpret_cast<int32_t*>(m_pData->data() + m_iLastPrepare + 1);
	*pTarget = htonl(m_iWritten - m_iLastPrepare - 1);

	m_iLastPrepare = m_iWritten;
}

PgDataWriter& PgDataWriter::operator <<(int8_t value) {
	if(!check(1) ) {
		return *this;
	}
	int8_t* pTarget = reinterpret_cast<int8_t*>(m_pData->data() + m_iWritten);
	*pTarget = value;
	++m_iWritten;
	return *this;
}

bool PgDataWriter::addInt32(int32_t value) {
	if(!check(4)) {
		return false;
	}

	int32_t* pTarget = reinterpret_cast<int32_t*>(m_pData->data() + m_iWritten);
	*pTarget = htonl(value);

	m_iWritten += 4;
	return true;
}

PgDataWriter& PgDataWriter::operator <<(int64_t value) {
	int32_t* iValue = reinterpret_cast<int32_t*>(&value);
	addInt32(iValue[1]) && 	addInt32(iValue[0]);
	return *this;
}

PgDataWriter& PgDataWriter::operator <<(int16_t value) {
	if(!check(2)) {
		return *this;
	}

	int16_t* pTarget = reinterpret_cast<int16_t*>(m_pData->data() + m_iWritten);
	*pTarget =  htons(value);
	m_iWritten += 2;
	return *this;
}

bool PgDataWriter::addStringZeroEnd(const std::string_view s) {
	auto len = s.length();
	if(!check( len + 1) ){
		return false;
	}
	char* pTarget = reinterpret_cast<char*>(m_pData->data() + m_iWritten);
	memcpy(pTarget, s.data(), len);
	m_iWritten += len;
	pTarget[len] = '\0';
	++m_iWritten;

	return true;
}

constexpr auto DIGITS = "0123456789ABCDEF";

void PgDataWriter::addBytesString(const std::string_view s) {
	auto len = 2 + s.length() * 2;
	if(!addInt32(len) || !check(len)) {
		return;
	}
	char* pTarget = reinterpret_cast<char*>(m_pData->data());
	pTarget[m_iWritten++] = '\\';
	pTarget[m_iWritten++] = 'x';

	for (size_t i = 0; i   <  s.length() ; ++i) {
		uint8_t c = s[i];

		pTarget[m_iWritten++] = DIGITS[c >> 4];
		pTarget[m_iWritten++] = DIGITS[c & 0xf];
	}
}

void PgDataWriter::addString(const std::string_view s) {
	auto len = s.length();
	if(!addInt32(len)  || !check(len) ) {
		return;
	}
	char* pTarget = reinterpret_cast<char*>(m_pData->data() + m_iWritten);
	memcpy(pTarget, s.data(), len);
	m_iWritten += len;
}

void PgDataWriter::addDateTimeAsString(struct tm* pTime, const char* pszFormat, size_t len) {
	if(!addInt32(len) || !check(len+1) ) {
		return;
	}
	char* pTarget = reinterpret_cast<char*>(m_pData->data()+ m_iWritten);
	auto iWritten = strftime(pTarget, m_pData->size() - m_iWritten, pszFormat, pTime);
	assert(iWritten == len);
	m_iWritten += iWritten;
}
