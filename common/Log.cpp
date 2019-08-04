#include "Log.h"
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "common/IOException.h"

#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid) 

#define MAX_LOG_LEN 4096

Log::Log()
		: m_pszLogPath("log/server.log"), m_pLogFile(NULL), m_level(INFO), m_iDay(
				0)
{
  pthread_mutex_init(&m_lock, 0);
}

void Log::init(const char* pszPath, LogLevel level)
{
	m_level = level;
	m_pszLogPath = pszPath;
}

Log::~Log()
{
  pthread_mutex_destroy(&m_lock);
	if (m_pLogFile != NULL)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

FILE* Log::getFile(uint64_t iDay)
{
	if (m_pszLogPath == NULL)
		return stdout;

	if (m_pLogFile != NULL && m_iDay == iDay)
		return m_pLogFile;

	pthread_mutex_lock(&m_lock);
	if (m_pLogFile == NULL || m_iDay != iDay)
	{
		char szBuf[1024];
		snprintf(szBuf, 1024, "%s.%04lu-%02lu-%02lu", m_pszLogPath,
				(iDay >> 16) + 1900, ((iDay & 0xffff) >> 8) + 1, iDay & 0xff);
		FILE* pFile = fopen(szBuf, "a+");
		if (pFile == 0)
		{
			fprintf(stderr, "Failed to open log file %s!", szBuf);
			m_pszLogPath = NULL;
			pthread_mutex_unlock(&m_lock);
			return stdout;
		}
		if (m_pLogFile != NULL)
		{
			sleep(1); //wait for other thread's write
			fclose(m_pLogFile);
			m_pLogFile = NULL;
		}
		m_pLogFile = pFile;
		m_iDay = iDay;
	}
	pthread_mutex_unlock(&m_lock);
	return m_pLogFile;
}

void Log::log(LogLevel level, const char* pszPath, int iLine,
		const char* pszFormat, ...)
{
	if (m_level > level)
		return;

	time_t curtime = time(0);
	tm time = {0};
	localtime_r(&curtime, &time);


	const char* pszLevel = "UNKNOWN";
	switch (level)
	{
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

	//fprintf is thread safe
#ifndef NDEBUG
	fprintf(pFile, "%s %02d:%02d:%02d(%lu %s:%d) %s\n", pszLevel, time.tm_hour,
			time.tm_min, time.tm_sec, gettid(), pszPath, iLine, szBuf);
#else
	fprintf(pFile, "%s %02d:%02d:%02d(%lu) %s\n",
			pszLevel,
			time.tm_hour, time.tm_min, time.tm_sec,
			gettid(), szBuf);
#endif
	fflush(pFile);
}
