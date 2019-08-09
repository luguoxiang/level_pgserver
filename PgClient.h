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
	typedef void (PgClient::*MessageHandler)();
private:
	void describeColumn(ExecutionPlan* pPlan);
	void sendRow(ExecutionPlan* pPlan);

	void createPlan(const char* pszSql, size_t len);
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
	const char* m_pszSql;
	MessageHandler m_handler[100];
};
