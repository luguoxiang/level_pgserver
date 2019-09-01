#pragma once

#include <memory>
#include "PgMessageSender.h"
#include "PgMessageReceiver.h"

#include "execution/WorkloadResult.h"

class Exception;

class PgClient {
public:
	PgClient(WorkThreadInfo* pInfo);
	~PgClient();

	void run();
	using MessageHandler = void (PgClient::*)();
private:
	void describeColumn();
	void sendRow(ExecutionPlan* pPlan);

	void createPlan(const std::string_view sql);
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

	uint64_t m_iSendTime;
	MessageHandler m_handler[100];
};
