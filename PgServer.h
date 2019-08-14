#pragma once

#include "execution/WorkThreadInfo.h"

class PgServer {
public:
	PgServer(const std::string& sPort);
	~PgServer();
	void run();
private:
	static void worker_thread(WorkThreadInfo* pArg);

	int bindSocket(const std::string& sPort);

	//throw ObCommException
	static int acceptSocket(int fd, int maxConnection);

	const std::string m_sPort;

	int m_iFd;
};
