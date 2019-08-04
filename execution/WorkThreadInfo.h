#ifndef WORKTHREAD_INFO_H
#define WORKTHREAD_INFO_H
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "execution/ExecutionPlan.h"
#include "execution/HBaseConnection.h"
#include "execution/ObConnection.h"

struct WorkThreadInfo
{
	WorkThreadInfo(int fd, const char* pszPort, int iIndex);

	~WorkThreadInfo();

	void clearPlan()
	{
		for (size_t i = 0; i < m_plans.size(); ++i)
		{
			delete m_plans[i];
		}
		m_plans.clear();
	}

	static pthread_key_t tls_key;

	pthread_t m_pthread;
	int m_tid;
	int m_iListenFd;
	int m_iAcceptFd;
	const char* m_pszPort;

	bool m_bRunning;
	uint64_t m_iClientTime;
	int m_iIndex;
	int m_iSessions;

	uint64_t m_iExecScanTime;
	uint64_t m_iBiggestExec;
	int m_iSqlCount;
	ExecutionPlan* m_pPlan;

	//throws ParseException
	void parse(const char* pszSQL, size_t iLen);

	void print();

	ExecutionPlan* resolve();

	char* alloc(size_t iSize);

	char* memdup(const char* p, size_t len)
	{
		return my_memdup(&m_result, p, len);
	}

	void pushPlan(ExecutionPlan* pPlan)
	{
		assert(pPlan);
		m_plans.push_back(pPlan);
	}

	ExecutionPlan* popPlan()
	{
		if (m_plans.empty())
			return NULL;
		ExecutionPlan* pPlan = m_plans.back();
		m_plans.pop_back();
		return pPlan;
	}
	ObConnection& getOBConnection() 
	{
		return *ObConnection::getInstance();
	}

	HBaseConnection& getHBaseConnection()
	{
		if(m_pHBase == NULL)
		{
			m_pHBase = new HBaseConnection();
		}
		return *m_pHBase;
	}
private:
	HBaseConnection* m_pHBase;
	ParseResult m_result;
	std::vector<ExecutionPlan*> m_plans;
};

inline void* operator new[](size_t size, WorkThreadInfo& pool)
{
	return pool.alloc(size);
}

inline void* operator new(size_t size, WorkThreadInfo& pool)
{
	return pool.alloc(size);
}

class WorkerManager
{
public:
	static WorkerManager& getInstance();

	~WorkerManager();

	size_t getWorkerCount()
	{
		return m_workers.size();
	}

	WorkThreadInfo* getWorker(size_t i)
	{
		assert(i < m_workers.size());
		return m_workers[i];
	}

	void addWorker(WorkThreadInfo* pWorker)
	{
		m_workers.push_back(pWorker);
	}
private:
	WorkerManager()
	{
	}
	;
	std::vector<WorkThreadInfo*> m_workers;
};
#endif //WORKTHREAD_INFO_H
