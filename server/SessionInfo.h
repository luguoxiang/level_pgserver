#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include <asio.hpp>
#include <optional>

#include "common/ParseResult.h"
#include "execution/ExecutionPlan.h"
#include "PostgresProtocol.h"

using asio::ip::tcp;

class SessionInfo {
public:
	SessionInfo(int iIndex, asio::io_service& ioService);

	void cancel();



	int16_t getIndex() {return m_iIndex;}


	SessionInfo(const SessionInfo&) = delete;
	SessionInfo& operator =(const SessionInfo&) = delete;

	tcp::socket& init() {
		m_pSocket.emplace(m_ioService);
		return *m_pSocket;
	}
	void start();
	void end();

	static uint64_t getSqlCount() {
		return m_sqlCount;
	}
	static uint64_t getReadCount() {
		return m_readCount;
	}
	static uint64_t getWriteCount() {
		return m_writeCount;
	}
private:

	void readData(size_t len);
	void readMessageType();
	void readLen();

	void handleStartup(size_t len) ;
	void handleSync(size_t len) ;
	void handleQuery(size_t len) ;
	void handleParse(size_t len);
	void handleBind(size_t len);
	void handleDescription(size_t len);
	void handleExecution(size_t len);

	int m_iIndex;

	std::optional<ParseResult> m_pResult;

	PostgresProtocol m_protocol;

	//callback can throw exception
	std::map<char, void (SessionInfo::*)(size_t)> m_handler;

	ExecutionPlanPtr m_pPlan;

	std::atomic_bool m_bTerminate;



	void parse(const std::string_view sql);
	void resolve();



	void handleExecutionBegin();
	void handleExecutionResult();
	void handleExecutionDone();

	//callback should not throw exception
	void doRead(size_t len, std::function<bool(size_t len)> fn);

	//callback can throw exception
	void doWrite(size_t len, void (SessionInfo::*)());

	bool m_bBinded = false;
	bool m_bParsed = false;

	asio::io_service::strand m_strand;
	std::optional<tcp::socket> m_pSocket;
	asio::io_service& m_ioService;

    MemBlockPtr m_pBuffer;
	char m_qtype = 0;
	static thread_local uint64_t m_sqlCount;
	static thread_local uint64_t m_readCount;
	static thread_local uint64_t m_writeCount;
};


