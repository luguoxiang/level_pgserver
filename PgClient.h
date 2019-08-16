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
	void describeColumn(ExecutionPlan* pPlan);
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

	std::unique_ptr<ExecutionPlan> m_pPlan;

	uint64_t m_iSendTime;
	int m_iParamNum;
	MessageHandler m_handler[100];
};
