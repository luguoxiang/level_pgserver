#include "SessionManager.h"

void SessionManager::stop() {
	std::lock_guard < std::mutex > lock(m_mutex);

	for(auto& pWorker : m_sessions) {
		pWorker->cancel();
	}
}

void SessionManager::clear() {
	std::lock_guard < std::mutex > lock(m_mutex);

	LOG(INFO)<< "removing "<< m_sessions.size() <<" session objects";
	m_sessions.clear();
}

SessionInfo* SessionManager::allocSession(asio::io_service& ioService) {
	std::lock_guard < std::mutex > lock(m_mutex);

	size_t iIndex = 0;
	if(m_free.empty()) {
		iIndex = m_sessions.size();
		m_sessions.emplace_back(new SessionInfo(iIndex, ioService));
	} else {
		iIndex = m_free.back();
		m_free.pop_back();
	}
	DLOG(INFO)<< "Active sessions:" << m_sessions.size() - m_free.size() <<", free:" <<  m_free.size();
	return m_sessions[iIndex].get();
}

void SessionManager::freeSession(SessionInfo* pWorker) {
	std::lock_guard < std::mutex > lock(m_mutex);

	m_free.push_back(pWorker->getIndex());

	DLOG(INFO)<< "Active sessions:" << m_sessions.size() - m_free.size() <<", free:" <<  m_free.size();
}

size_t SessionManager::getSessionCount() {
	std::lock_guard < std::mutex > lock(m_mutex);
	return m_sessions.size();
}

SessionInfo* SessionManager::getSession(size_t i) {
	std::lock_guard < std::mutex > lock(m_mutex);

	assert(i < m_sessions.size());
	return m_sessions[i].get();
}
