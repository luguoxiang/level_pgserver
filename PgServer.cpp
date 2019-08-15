#include <thread>
#include <vector>
#include <csignal>

#include <unistd.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <glog/logging.h>

#include "common/MetaConfig.h"
#include "PgMessageSender.h"

#include "PgServer.h"
#include "PgClient.h"
#include "execution/ExecutionException.h"
#include "execution/WorkThreadInfo.h"



#define MAX_CONNECTION 1000

PgServer::PgServer(int port) :
		m_port(port), m_iFd(-1) {
	LOG(INFO) << "Start server on port "<< port;
}

PgServer::~PgServer() {
	if (m_iFd >= 0) {
		::close(m_iFd);
	}
}

int PgServer::acceptSocket(int fd, int maxConnection) {
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	struct sockaddr* pAddr = (struct sockaddr*) &addr;

	int acceptSock = ::accept(fd, pAddr, &addrlen);

	if (acceptSock < 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		throw new IOException("accept() failed");
	}

	struct sockaddr_in* pAddrIn = (struct sockaddr_in*) pAddr;
	const char* pszIP = ::inet_ntoa(pAddrIn->sin_addr);

	int iTimeout = MetaConfig::getInstance().getTimeout();
	DLOG(INFO)<< "Accept client:" <<pszIP <<", timeout=" << iTimeout;
	int optval = 1;
	if (::setsockopt(acceptSock, IPPROTO_TCP, TCP_NODELAY, (char*) &optval,
			sizeof(optval)) < 0) {
		::close(acceptSock);
		throw new IOException("setsockopt(TCP_NODELAY) failed");
	}
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_KEEPALIVE, (char*) &optval,
			sizeof(optval)) < 0) {
		::close(acceptSock);
		throw new IOException("setsockopt(SO_KEEPALIVE) failed");
	}

	struct timeval timeout;
	timeout.tv_sec = iTimeout;
	timeout.tv_usec = 0;
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(timeout)) < 0) {
		::close(acceptSock);
		throw new IOException("setsockopt(SO_SNDTIMEO) failed");
	}
	if (::setsockopt(acceptSock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
			sizeof(timeout)) < 0) {
		::close(acceptSock);
		throw new IOException("setsockopt(SO_RCVTIMEO) failed");
	}
	return acceptSock;
}

int PgServer::bindSocket(int port) {
	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throw new IOException("Could not create socket()!");
	}

	int optval = 1;

	struct sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &optval,
			sizeof(optval)) < 0) {
		::close(fd);
		throw new IOException("could not setsockopt(SO_REUSEADDR)");
	}
	int flags = ::fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		::close(fd);
		throw new IOException("could not fcntl(F_GETFL)");
	}
	if (::bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		::close(fd);
		throw new IOException("could not bind socket");
	}
	return fd;
}

void PgServer::worker_thread(WorkThreadInfo* pInfo) {
	pInfo->m_tid = std::this_thread::get_id();
	WorkThreadInfo::m_pWorkThreadInfo = pInfo;
	LOG(INFO) << "Working thread is listening on " << pInfo->m_port;
	while (true) {
		try {
			pInfo->m_iAcceptFd = acceptSocket(pInfo->m_iListenFd,
			MAX_CONNECTION);
		} catch (IOException* pe) {
			LOG(ERROR) << "Working thread failed:" << pe->what();
			delete pe;
			exit(0);
		}
		pInfo->m_bRunning = true;
		++pInfo->m_iSessions;

		try {
			PgClient client(pInfo);
			client.run();
		} catch (Exception* pe) {
			LOG(ERROR) << "Working thread failed:" << pe->what();
			delete pe;
		} catch (...) {
			LOG(ERROR) << "Working thread failed:Unknown Reason.";
		}
		pInfo->m_bRunning = false;
	}
	LOG(WARNING) << "Working thread terminate.";
}

static void int_handler(int code) {
	LOG(INFO) << "Receive INT signal!";
	exit(0);
}

void PgServer::run() {
	std::signal(SIGINT, int_handler);

	m_iFd = bindSocket(m_port);

	if (::listen(m_iFd, MAX_CONNECTION) < 0) {
		::close(m_iFd);
		throw new IOException("could not listen!");
	}

	int iWorkerNum = MetaConfig::getInstance().getWorkerNum();

	std::vector < std::thread > threads(iWorkerNum);
	for (uint32_t i = 0; i < iWorkerNum; ++i) {
		WorkThreadInfo* pInfo = new WorkThreadInfo(m_iFd, m_port, i);
		threads[i] = std::thread(worker_thread, pInfo);
		WorkerManager::getInstance().addWorker(pInfo);
	}

	for (auto& th : threads) {
		th.join();
	}
	LOG(INFO) << "Main Server shutdown!";
}
