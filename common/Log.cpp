#include "Log.h"
#include <thread>
#include <sstream>
#include <chrono>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "common/IOException.h"

namespace {
constexpr size_t MAX_LOG_LEN = 4096;
}

Log::Log() : m_sLogPath("log/server.log"), m_pLogFile(nullptr), m_level(LogLevel::INFO), m_iDay(0) {
}

void Log::init(const std::string& sPath, LogLevel level) {
	m_level = level;
	m_sLogPath = sPath;
}

Log::~Log() {
	if (m_pLogFile != nullptr) {
		fclose(m_pLogFile);
		m_pLogFile = nullptr;
	}
}

FILE* Log::getFile(uint64_t iDay) {
	if (m_sLogPath == "")
		return stdout;

	if (m_pLogFile != nullptr && m_iDay == iDay)
		return m_pLogFile;

	std::lock_guard < std::mutex > guard(m_lock);
	if (m_pLogFile == nullptr || m_iDay != iDay) {
		char szBuf[1024];
		snprintf(szBuf, 1024, "%s.%04llu-%02llu-%02llu", m_sLogPath.c_str(),
				(iDay >> 16) + 1900, ((iDay & 0xffff) >> 8) + 1, iDay & 0xff);
		FILE* pFile = fopen(szBuf, "a+");
		if (pFile == 0) {
			fprintf(stderr, "Failed to open log file %s!", szBuf);
			m_sLogPath = "";
			return stdout;
		}
		if (m_pLogFile != nullptr) {
			sleep(1); //wait for other thread's write
			fclose(m_pLogFile);
			m_pLogFile = nullptr;
		}
		m_pLogFile = pFile;
		m_iDay = iDay;
	}
	return m_pLogFile;
}

void Log::log(LogLevel level, const std::string& sPath, int iLine,
		const char* pszFormat, ...) {
	if (m_level > level)
		return;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	tm time = *localtime(&tt);

	const char* pszLevel = "UNKNOWN";
	switch (level) {
	case LogLevel::WARN:
		pszLevel = "WARN";
		break;
	case LogLevel::INFO:
		pszLevel = "INFO";
		break;
	case LogLevel::DEBUG:
		pszLevel = "DEBUG";
		break;
	case LogLevel::ERROR:
		pszLevel = "ERROR";
		break;
	default:
		assert(0);
		break;
	}
	va_list var_args;

	uint64_t iDay = (time.tm_year << 16) + (time.tm_mon << 8) + time.tm_mday;
	FILE* pFile = getFile(iDay);

	char szBuf[MAX_LOG_LEN];
	va_start(var_args, pszFormat);
	vsnprintf(szBuf, MAX_LOG_LEN, pszFormat, var_args);
	va_end(var_args);

	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string tid = ss.str();
	//fprintf is thread safe
#ifndef NDEBUG
	fprintf(pFile, "%s %02d:%02d:%02d(%s %s:%d) %s\n", pszLevel, time.tm_hour,
			time.tm_min, time.tm_sec, tid.c_str(), sPath.c_str(), iLine, szBuf);
#else
	fprintf(pFile, "%s %02d:%02d:%02d(%s) %s\n",
			pszLevel,
			time.tm_hour, time.tm_min, time.tm_sec,
			tid.c_str(), szBuf);
#endif
	fflush(pFile);
}
