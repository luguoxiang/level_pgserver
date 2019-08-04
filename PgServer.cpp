#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/ioctl.h>

#include "common/MetaConfig.h"
#include "PgMessageSender.h"

#include "PgServer.h"
#include "PgClient.h"
#include "common/Log.h"
#include "execution/ExecutionException.h"
#include "execution/WorkThreadInfo.h"

#include <iostream>

#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid) 

#define MAX_CONNECTION 1000


PgServer::PgServer(const char* pszPort)
		: m_pszPort(pszPort), m_iFd(-1)
{
	LOG(INFO, "Start server on port %s!", pszPort);
}

PgServer::~PgServer()
{
	if (m_iFd >= 0)
	{
		close(m_iFd);
	}
}

void PgServer::mysleep(long microsec)
{
	struct timeval delay;
	delay.tv_sec = microsec / 1000000L;
	delay.tv_usec = microsec % 1000000L;
	select(0, NULL, NULL, NULL, &delay);
}

int PgServer::acceptSocket(int fd, int maxConnection)
{
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	struct sockaddr* pAddr = (struct sockaddr*) &addr;

	int acceptSock = ::accept(fd, pAddr, &addrlen);

	if (acceptSock < 0)
	{
		//sleep 0.1 sec
		mysleep(100000);
		throw new IOException("accept() failed");
	}

	struct sockaddr_in* pAddrIn = (struct sockaddr_in*) pAddr;
	const char* pszIP = ::inet_ntoa(pAddrIn->sin_addr);

	int iTimeout = MetaConfig::getInstance().getTimeout();
	LOG(DEBUG, "Accept client:%s, timeout=%d!", pszIP, iTimeout);
	int optval = 1;
	if (::setsockopt(acceptSock, IPPROTO_TCP, TCP_NODELAY, (char*) &optval,
			sizeof(optval)) < 0)
	{
		::close(acceptSock);
		throw new IOException("setsockopt(TCP_NODELAY) failed", pszIP);
	}
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_KEEPALIVE, (char*) &optval,
			sizeof(optval)) < 0)
	{
		::close(acceptSock);
		throw new IOException("setsockopt(SO_KEEPALIVE) failed", pszIP);
	}

	struct timeval timeout;
	timeout.tv_sec = iTimeout;
	timeout.tv_usec = 0;
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(timeout)) < 0)
	{
		::close(acceptSock);
		throw new IOException("setsockopt(SO_SNDTIMEO) failed", pszIP);
	}
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
			sizeof(timeout)) < 0)
	{
		::close(acceptSock);
		throw new IOException("setsockopt(SO_RCVTIMEO) failed", pszIP);
	}
	return acceptSock;
}

int PgServer::bindSocket(const char* pszPort)
{
	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		throw new IOException("Could not create socket()!");
	}

	int optval = 1;

	struct sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(pszPort));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &optval,
			sizeof(optval)) < 0)
	{
		::close(fd);
		throw new IOException("could not setsockopt(SO_REUSEADDR)");
	}
	int flags = ::fcntl(fd, F_GETFL, 0);
	if (flags < 0)
	{
		::close(fd);
		throw new IOException("could not fcntl(F_GETFL)");
	}
	if (::bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
		::close(fd);
		throw new IOException("could not bind socket");
	}
	return fd;
}

void* PgServer::worker_thread(void* pArg)
{
	WorkThreadInfo* pInfo = (WorkThreadInfo*) pArg;
	pthread_setspecific(WorkThreadInfo::tls_key, pInfo);
	pInfo->m_tid = gettid();
	LOG(INFO, "Working thread is listening on %s.", pInfo->m_pszPort);
	while (true)
	{
		try
		{
			pInfo->m_iAcceptFd = acceptSocket(pInfo->m_iListenFd,
					MAX_CONNECTION);
		} catch (IOException* pe)
		{
			LOG(ERROR, "Working thread failed:%s.", pe->what());
			delete pe;
			exit(0);
		}
		pInfo->m_bRunning = true;
		++pInfo->m_iSessions;

		try
		{
			PgClient client(pInfo);
			client.run();
		} catch (Exception* pe)
		{
			LOG(ERROR, "Working thread failed:%s.", pe->what());
			delete pe;
		} catch (...)
		{
			LOG(ERROR, "Working thread failed:Unknown Reason.");
		}
		pInfo->m_bRunning = false;
	}LOG(WARN, "Working thread terminate.");
	return NULL;
}

static void int_handler(int code)
{
	LOG(INFO, "Receive INT signal!");
	exit(0);
}

void PgServer::run()
{
	struct sigaction act;
	act.sa_handler = int_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		LOG(ERROR, "Could not set SIGINT handler!");
	}

	m_iFd = bindSocket(m_pszPort);

	if (::listen(m_iFd, MAX_CONNECTION) < 0)
	{
		::close(m_iFd);
		throw new IOException("could not listen!");
	}

	pthread_key_create(&WorkThreadInfo::tls_key, NULL);

	int iWorkerNum = MetaConfig::getInstance().getWorkerNum();
	for (uint32_t i = 0; i < iWorkerNum; ++i)
	{
		WorkThreadInfo* pInfo = new WorkThreadInfo(m_iFd, m_pszPort, i);
		if (::pthread_create(&(pInfo->m_pthread), NULL, worker_thread,
				(void *) pInfo) != 0)
		{
			::close(m_iFd);
			throw new IOException("Failed to create thread!");
		}
		WorkerManager::getInstance().addWorker(pInfo);
	}

	for (size_t i = 0; i < WorkerManager::getInstance().getWorkerCount(); ++i)
	{
		void* pResult = 0;
		WorkThreadInfo* pInfo = WorkerManager::getInstance().getWorker(i);
		::pthread_join(pInfo->m_pthread, &pResult);
	}LOG(INFO, "Main Server shutdown!");
}
