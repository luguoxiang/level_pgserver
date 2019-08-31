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
	void parse(const std::string_view sql);

	void print();

	ExecutionPlanPtr resolve();
	void cancel() {
		if (m_pPlan != nullptr){
			m_pPlan->cancel();
		}
	}

	void pushPlan(ExecutionPlanPtr& pPlan) {
		assert(pPlan.get());
		m_plans.push_back(pPlan);
	}

	ExecutionPlanPtr popPlan() {
		if (m_plans.empty())
			return nullptr;
		ExecutionPlanPtr pPlan = m_plans.back();
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
	void setPlan(ExecutionPlanPtr& pPlan) {
		m_pPlan = pPlan;
	}

	void clearPlan() {
		m_pPlan = nullptr;
	}
private:
	ExecutionPlanPtr m_pPlan = nullptr;
	ParseResult m_result;
	std::vector<ExecutionPlanPtr> m_plans;
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
