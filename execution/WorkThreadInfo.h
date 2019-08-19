#pragma once
#include <stdio.h>
#include <vector>
#include <thread>
#include <bitset>
#include "execution/ExecutionPlan.h"
#include "execution/ExecutionException.h"

class ExecutionBuffer
{
public:
	ExecutionBuffer(size_t size): m_executionBuffer(size) {};

	using Row = std::byte*;

	Row beginRow() {
		m_nullBits.reset();
		m_iIndex = 0;
		m_pNullBitsLong = alloc<unsigned long>(0);
		return reinterpret_cast<Row>(m_pNullBitsLong);
	}
	void allocForColumn(DBDataType type, const ExecutionResult& result);

	void endRow() {
		*m_pNullBitsLong = m_nullBits.to_ulong();
	}
	void purge() {
		m_iUsed = 0;
	}

	void getResult(Row row, size_t index, ExecutionResult& result, const std::vector<DBDataType>& types);
	int compare(Row row1, Row row2, size_t index, const std::vector<DBDataType>& types);

private:
	template <class T>
	T* alloc(T t) {
		T* data = reinterpret_cast<T*>(m_executionBuffer.data() + m_iUsed);
		m_iUsed += sizeof(T);
		if (m_iUsed > m_executionBuffer.size()) {
			throw new ExecutionException("not enough execution buffer");
		}
		*data = t;
		return data;
	}
	std::byte* get(Row row, size_t index, const std::vector<DBDataType>& types);

	std::string_view allocString(std::string_view t);

	std::vector<std::byte> m_executionBuffer;

	size_t m_iUsed = 0;
	std::bitset<32> m_nullBits;
	unsigned long* m_pNullBitsLong;
	size_t m_iIndex;
};

struct WorkThreadInfo {
	WorkThreadInfo(int fd, int port, int iIndex,size_t executionBufferSize);

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
	ExecutionBuffer& getExecutionBuffer() {return m_executionBuffer;}
private:
	ParseResult m_result;
	std::vector<std::unique_ptr<ExecutionPlan>> m_plans;
	ExecutionBuffer m_executionBuffer;
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
