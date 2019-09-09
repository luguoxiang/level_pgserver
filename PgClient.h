#pragma once

#include <memory>
#include <atomic>
#include "PgMessageSender.h"
#include "PgMessageReceiver.h"

#include "execution/WorkloadResult.h"

class Exception;
class WorkThreadInfo;
class PgClient {
public:
	PgClient(WorkThreadInfo* pInfo, std::atomic_bool& bGlobalTerminate);

	void run();

private:
	void resolve();
	void describeColumn();
	void sendRow();

	void handleException(Exception* pe);
	void handleSync();
	void handleQuery();
	void handleParse();
	void handleBind();
	void handleDescribe();
	void handleExecute();

	PgMessageReceiver m_receiver;
	PgMessageSender m_sender;

	WorkThreadInfo* m_pWorker;

	uint64_t m_iSendTime = 0;

	using MessageHandler = void (PgClient::*)();

	MessageHandler m_handler[100];

	ExecutionPlanPtr m_pPlan;

	std::atomic_bool& m_bGlobalTerminate;
	bool m_bDescribed = false;
};
