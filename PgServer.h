#pragma once

#include "execution/WorkThreadInfo.h"

class PgServer {
public:
	PgServer(int sPort);
	~PgServer();
	void run();
private:
	static void worker_thread(WorkThreadInfo* pArg);

	int bindSocket(int port);

	//throw ObCommException
	static int acceptSocket(int fd, int maxConnection);

	const int m_port;

	int m_iFd;
};
