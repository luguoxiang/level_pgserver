#ifndef OBSERVER_H
#define OBSERVER_H
#include "execution/ObConnection.h"

class PgServer
{
public:
	PgServer(const char* pszPort);
	~PgServer();
	void run();
private:
	static void* worker_thread(void* pArg);

	int bindSocket(const char* pszPort);

	//throw ObCommException
	static int acceptSocket(int fd, int maxConnection);

	static void mysleep(long microsec);

	const char* m_pszPort;

	int m_iFd;
};
#endif
