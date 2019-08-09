#pragma once

#include "execution/WorkThreadInfo.h"

class PgServer
{
public:
	PgServer(const char* pszPort);
	~PgServer();
	void run();
private:
	static void worker_thread(WorkThreadInfo* pArg);

	int bindSocket(const char* pszPort);

	//throw ObCommException
	static int acceptSocket(int fd, int maxConnection);

	const char* m_pszPort;

	int m_iFd;
};
