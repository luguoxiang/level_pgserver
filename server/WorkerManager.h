#pragma once

#include "WorkThreadInfo.h"

class WorkerManager {
public:

	static WorkerManager& getInstance() {
		static WorkerManager manager;
		return manager;
	}

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

	void cancel(bool planOnly);

	WorkerManager(const WorkerManager&) = delete;
	WorkerManager& operator =(const WorkerManager&) = delete;
private:
	WorkerManager() {}

	std::vector<std::unique_ptr<WorkThreadInfo>> m_workers;
};
