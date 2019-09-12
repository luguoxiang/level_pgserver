#pragma once

#include <memory>
#include <atomic>
#include "PostgresProtocol.h"

#include "execution/WorkloadResult.h"

class Exception;
class WorkThreadInfo;
class PgClient {
public:
	PgClient(WorkThreadInfo* pInfo, std::atomic_bool& bGlobalTerminate);

	void run();

private:
	void resolve();
	void sendRow();

	void handleQuery();
	void handleParse();
	void handleBind();
	void handleExecute();

	PostgresProtocol m_protocol;

	WorkThreadInfo* m_pWorker;

	uint64_t m_iSendTime = 0;

	std::function<void ()> m_handler[100];

	ExecutionPlanPtr m_pPlan;

	std::atomic_bool& m_bGlobalTerminate;
};
