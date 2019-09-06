#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include "common/ParseResult.h"
#include "common/QueryRewritter.h"

struct WorkThreadInfo {
	WorkThreadInfo(int iIndex);

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
	static bool isCanceled() {
		return m_pWorkThreadInfo->m_bTerminate.load();
	}

	bool m_bRunning = false;
	uint64_t m_iClientTime = 0;
	int m_iSessions = 0;

	void parse(const std::string_view sql);

	ParseNode* getParseTree() {
		return m_result.m_pResult;
	}

	void print();

	void cancel(bool planOnly);

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

	void setAcceptFd(int fd);

	int getAcceptFd() {
		return m_iAcceptFd;
	}
	int getIndex() {return m_iIndex;}
	int getSqlCount() {return m_iSqlCount;}
private:
	int m_iIndex;
	int m_iSqlCount = 0;

	int m_iAcceptFd = 0;
	ParseResult m_result;
	QueryRewritter m_rewritter;
	static thread_local WorkThreadInfo *m_pWorkThreadInfo;


	std::atomic_bool m_bTerminate;
};



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

	void cancel(bool planOnly) {
		for(auto& pWorker : m_workers) {
			pWorker->cancel(planOnly);
		}
	}

private:
	WorkerManager() {
	}

	std::vector<std::unique_ptr<WorkThreadInfo>> m_workers;
};
