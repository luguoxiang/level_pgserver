#pragma once
#include <stdio.h>
#include <vector>
#include <thread>
#include "execution/ExecutionPlan.h"

struct WorkThreadInfo {
	WorkThreadInfo(int fd, int port, int iIndex);

	~WorkThreadInfo();

	void clearPlan() {
		m_plans.clear();
	}

	static thread_local WorkThreadInfo *m_pWorkThreadInfo;

	std::thread::id m_tid;

	int m_iListenFd;
	int m_iAcceptFd;
	int m_port;

	bool m_bRunning;
	uint64_t m_iClientTime;
	int m_iIndex;
	int m_iSessions;

	uint64_t m_iExecScanTime;
	uint64_t m_iBiggestExec;
	int m_iSqlCount;
	ExecutionPlan* m_pPlan;

	//throws ParseException
	void parse(const std::string& sql);

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
