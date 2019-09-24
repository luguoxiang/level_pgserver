#include <thread>
#include <vector>

#include <glog/logging.h>

#include "common/MetaConfig.h"
#include "execution/ExecutionException.h"

#include "PgServer.h"
#include "SessionInfo.h"
#include "SessionManager.h"

PgServer::PgServer(int port)
	: m_port(port)
	, m_acceptor(m_ioService, tcp::endpoint(tcp::v4(), port)) {
	LOG(INFO)<< "Start server on port "<< m_port;
}

void PgServer::startAccept() {

	auto pWorker = SessionManager::getInstance().allocSession(m_ioService);
	m_acceptor.async_accept(pWorker->init(),
			[this, pWorker] (const std::error_code& ex) {
				if(ex) {
					LOG(INFO) << "accept connection failed: " << ex.message();
					pWorker->end();
				} else {
					LOG(INFO) << "accept connection";
					try {
						pWorker->start();
					} catch (std::exception& e) {
						pWorker->end();
					}
				}
				startAccept();
			});

}

void PgServer::run() {
	asio::signal_set signals(m_ioService, SIGINT, SIGTERM);
	signals.async_wait([this](auto, auto) {
		LOG(INFO)<< "Prepare server shutdown";
		m_acceptor.close();
		m_ioService.stop();
		SessionManager::getInstance().stop();
	});


	int iWorkerNum = MetaConfig::getInstance().getWorkerNum();
	startAccept();
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < iWorkerNum; ++i) {

		threads.emplace_back([this] () {
			LOG(INFO)<< "Working thread started";

			m_ioService.run();

			LOG(WARNING) << "Working thread terminate, sql="
						<<SessionInfo::getSqlCount() << ", read="
						<<SessionInfo::getReadCount() << ", write="
						<<SessionInfo::getWriteCount() ;
		});
	}

	for (auto& th : threads) {
		th.join();
	}
	SessionManager::getInstance().clear();
	LOG(INFO)<< "Main Server shutdown!";
}
