#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include "ExecutionPlan.h"
#include "common/ParseResult.h"

struct WorkThreadInfo {
	WorkThreadInfo(int fd, int port, int iIndex);

	~WorkThreadInfo();

	void clearPlan() {
		m_plans.clear();
	}

	static thread_local WorkThreadInfo *m_pWorkThreadInfo;

	std::thread::id m_tid;

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
	ExecutionPlan* m_pPlan = nullptr;

	//throws ParseException
	void parse(const std::string_view sql);

	void print();

	ExecutionPlan* resolve();


	void pushPlan(ExecutionPlan* pPlan) {
		assert(pPlan);
		m_plans.emplace_back(pPlan);
	}

	ExecutionPlan* popPlan() {
		if (m_plans.empty())
			return nullptr;
		ExecutionPlan* pPlan = m_plans.back().release();
		m_plans.pop_back();
		return pPlan;
	}

	size_t getBindParamNumber() {return m_result.m_bindParamNodes.size(); }
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
private:
	ParseResult m_result;
	std::vector<std::unique_ptr<ExecutionPlan>> m_plans;

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
