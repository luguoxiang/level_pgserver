#pragma once

#include <memory>
#include <atomic>
#include "PgMessageSender.h"
#include "PgMessageReceiver.h"

#include "execution/WorkloadResult.h"

class Exception;

class PgClient {
public:
	PgClient(WorkThreadInfo* pInfo, std::atomic_bool& bTerminate);
	~PgClient();

	void run();

	using MessageHandler = void (PgClient::*)();
private:
	void describeColumn();
	void sendRow(ExecutionPlan* pPlan);

	void handleException(Exception* pe);
	void handleSync();
	void handleQuery();
	void handleParse();
	void handleBind();
	void handleDescribe();
	void handleExecute();

	std::atomic_bool& m_bTerminate;
	PgMessageReceiver m_receiver;
	PgMessageSender m_sender;

	WorkThreadInfo* m_pWorker;

	uint64_t m_iSendTime = 0;
	MessageHandler m_handler[100];
};
