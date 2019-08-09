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

#define MAX_LOG_LEN 4096

Log::Log() :
		m_pszLogPath("log/server.log"), m_pLogFile(NULL), m_level(INFO), m_iDay(
				0) {
}

void Log::init(const char* pszPath, LogLevel level) {
	m_level = level;
	m_pszLogPath = pszPath;
}

Log::~Log() {
	if (m_pLogFile != NULL) {
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

FILE* Log::getFile(uint64_t iDay) {
	if (m_pszLogPath == NULL)
		return stdout;

	if (m_pLogFile != NULL && m_iDay == iDay)
		return m_pLogFile;

	std::lock_guard < std::mutex > guard(m_lock);
	if (m_pLogFile == NULL || m_iDay != iDay) {
		char szBuf[1024];
		snprintf(szBuf, 1024, "%s.%04llu-%02llu-%02llu", m_pszLogPath,
				(iDay >> 16) + 1900, ((iDay & 0xffff) >> 8) + 1, iDay & 0xff);
		FILE* pFile = fopen(szBuf, "a+");
		if (pFile == 0) {
			fprintf(stderr, "Failed to open log file %s!", szBuf);
			m_pszLogPath = NULL;
			return stdout;
		}
		if (m_pLogFile != NULL) {
			sleep(1); //wait for other thread's write
			fclose(m_pLogFile);
			m_pLogFile = NULL;
		}
		m_pLogFile = pFile;
		m_iDay = iDay;
	}
	return m_pLogFile;
}

void Log::log(LogLevel level, const char* pszPath, int iLine,
		const char* pszFormat, ...) {
	if (m_level > level)
		return;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	tm time = *localtime(&tt);

	const char* pszLevel = "UNKNOWN";
	switch (level) {
	case WARN:
		pszLevel = "WARN";
		break;
	case INFO:
		pszLevel = "INFO";
		break;
	case DEBUG:
		pszLevel = "DEBUG";
		break;
	case ERROR:
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
			time.tm_min, time.tm_sec, tid.c_str(), pszPath, iLine, szBuf);
#else
	fprintf(pFile, "%s %02d:%02d:%02d(%s) %s\n",
			pszLevel,
			time.tm_hour, time.tm_min, time.tm_sec,
			tid.c_str(), szBuf);
#endif
	fflush(pFile);
}
