#pragma once

#include <asio.hpp>
using asio::ip::tcp;

class WorkThreadInfo;

class PgServer {
public:
	PgServer(int port);

	void run();
	void startAccept();

private:
	const int m_port;


	asio::io_service m_ioService;
	tcp::acceptor m_acceptor;


};
