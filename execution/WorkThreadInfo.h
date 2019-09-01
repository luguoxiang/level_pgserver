#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include "ExecutionPlan.h"
#include "common/ParseResult.h"

struct WorkThreadInfo {
	WorkThreadInfo(int fd, int port, int iIndex);

	~WorkThreadInfo();

	static WorkThreadInfo* getThreadInfo() {
		return m_pWorkThreadInfo;
	}

	static ParseResult& getParseResult() {
		return m_pWorkThreadInfo->m_result;
	}

	static void setThreadInfo(WorkThreadInfo* pInfo) {
		m_pWorkThreadInfo = pInfo;
	}

	int m_iListenFd;
	int m_iAcceptFd = 0;
	int m_port;

	bool m_bRunning = false;
	uint64_t m_iClientTime = 0;
	int m_iIndex;
	int m_iSessions = 0;

	uint64_t m_iExecScanTime = 0;
	uint64_t m_iBiggestExec = 0;
	int m_iSqlCount = 0;


	//throws ParseException
	void resolve(const std::string_view sql);

	void print();

	void cancel() {
		if (m_pPlan != nullptr){
			m_pPlan->cancel();
		}
	}

	size_t getBindParamNumber() {
		return m_result.m_bindParamNodes.size();
	}

	ParseNode* getBindParam(size_t i) {
		assert(i < getBindParamNumber());
		return m_result.m_bindParamNodes[i];
	}

	std::string_view allocString(std::string_view s) {
		size_t len = s.length();
		char* alloc = m_result.alloc(len);

		std::copy(s.data(), s.data() + len, alloc);
		return std::string_view(alloc, len);
	}

	void markParseBuffer() {m_result.mark(); }
	void restoreParseBuffer() {m_result.restore(); }

	void clearPlan() {
		m_pPlan = nullptr;
	}

	ExecutionPlan* getPlan() {
		return m_pPlan.get();
	}
private:
	ExecutionPlanPtr m_pPlan = nullptr;
	ParseResult m_result;
	static thread_local WorkThreadInfo *m_pWorkThreadInfo;
};



class WorkerManager {
public:
	static WorkerManager& getInstance();


	size_t getWorkerCount() {
		return m_workers.size();
	}

	WorkThreadInfo* getWorker(size_t i) {
		assert(i < m_workers.size());
		return m_workers[i].get();
	}

	void addWorker(WorkThreadInfo* pWorker) {
		m_workers.emplace_back(pWorker);
	}

private:
	WorkerManager() {
	}

	std::vector<std::unique_ptr<WorkThreadInfo>> m_workers;
};
