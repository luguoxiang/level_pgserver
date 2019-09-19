#pragma once

#include "SessionInfo.h"
#include <mutex>

class SessionManager {
public:

	static SessionManager& getInstance() {
		static SessionManager manager;
		return manager;
	}

	size_t getSessionCount() ;

	SessionInfo* getSession(size_t i);

	SessionInfo* allocSession( asio::io_service& ioService);

	void freeSession(SessionInfo* pWorker);

	void stop();

	void clear();

	SessionManager(const SessionManager&) = delete;
	SessionManager& operator =(const SessionManager&) = delete;
private:
	SessionManager() {}

	std::vector<std::unique_ptr<SessionInfo>> m_sessions;
	std::vector<size_t> m_free;

	std::mutex m_mutex;
};
