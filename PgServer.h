#pragma once

#include <atomic>
#include "execution/WorkThreadInfo.h"

class PgServer {
public:
	static PgServer& getInstance() {
		static PgServer server;
		return server;
	}

	~PgServer();

	void run();
	void terminate();
private:
	PgServer();

	void worker_thread(WorkThreadInfo* pArg);

	static void int_handler(int code);
	int bindSocket(int port);

	int acceptSocket();

	const int m_port;

	int m_iFd = -1;

	std::atomic_bool m_bTerminate;
};
