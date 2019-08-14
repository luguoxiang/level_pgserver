#pragma once
#include <mutex>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define LOG(LEVEL, FORMAT, ...) \
	Log::getLogger().log(LogLevel::LEVEL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); 

enum class LogLevel {
    DEBUG, INFO, WARN, ERROR
};
class Log {
public:
	~Log();

	void init(const std::string& sPath, LogLevel level);

	static Log& getLogger() {
		static Log log;
		return log;
	}

	LogLevel getLevel() {
		return m_level;
	}

	void log(LogLevel level, const std::string& sPath, int iLine,
			const char* pszFormat, ...);
private:
	Log();

	FILE* getFile(uint64_t iDay);

	std::string m_sLogPath;

	FILE* m_pLogFile;
	LogLevel m_level;
	uint64_t m_iDay;
	std::mutex m_lock;
};

