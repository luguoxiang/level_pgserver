#pragma once
#include <mutex>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define LOG(LEVEL, FORMAT, ...) \
	Log::getLogger().log(Log::LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); 

class Log {
public:
	enum LogLevel {
		DEBUG = 0, INFO, WARN, ERROR
	};
	~Log();

	void init(const char* pszPath, LogLevel level);

	static Log& getLogger() {
		static Log log;
		return log;
	}

	LogLevel getLevel() {
		return m_level;
	}

	void log(LogLevel level, const char* pszPath, int iLine,
			const char* pszFormat, ...);
private:
	Log();

	FILE* getFile(uint64_t iDay);

	const char* m_pszLogPath;

	FILE* m_pLogFile;
	LogLevel m_level;
	uint64_t m_iDay;
	std::mutex m_lock;
};

