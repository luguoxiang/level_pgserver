#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "common/Log.h"
#include "execution/ExecutionException.h"
#include <assert.h>
#include "execution/ParseTools.h"
#include "common/MetaConfig.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = NULL;

WorkThreadInfo::WorkThreadInfo(int fd, const char* pszPort, int iIndex)
		: m_iListenFd(fd), m_iAcceptFd(0), m_pszPort(pszPort), 
		m_bRunning(false), m_iClientTime(0), m_iIndex(iIndex),m_iSessions(0), 
		m_iExecScanTime(0), m_iBiggestExec(0), 
		m_iSqlCount(0), m_pPlan(NULL)
{
	memset(&m_result, 0, sizeof(m_result));

	if (parseInit(&m_result))
	{
		throw new ParseException("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo()
{
	parseTerminate(&m_result);
	clearPlan();
}

void WorkThreadInfo::parse(const char* pszSQL, size_t iLen)
{
	parseSql(&m_result, pszSQL, iLen);

	if (m_result.m_pResult == 0)
	{
		throw new ParseException(&m_result);
	}
}

void WorkThreadInfo::print()
{
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}

ExecutionPlan* WorkThreadInfo::resolve()
{
	assert(m_result.m_pResult);

	BUILD_PLAN(m_result.m_pResult);

	ExecutionPlan* pPlan = popPlan();
	assert(pPlan && m_plans.empty());
	return pPlan;
}

char* WorkThreadInfo::alloc(size_t iSize)
{
	if (memPoolUsed(&m_result) + iSize
			> MetaConfig::getInstance().getExecutionBuffer())
	{
		throw new ExecutionException("execution memory usage exceed limitation",
				false);
	}
	return memPoolAlloc(iSize, &m_result);
}

WorkerManager& WorkerManager::getInstance()
{
	static WorkerManager manager;
	return manager;
}

WorkerManager::~WorkerManager()
{
	for (size_t i = 0; i < m_workers.size(); ++i)
	{
		if (m_workers[i] != NULL)
		{
			delete m_workers[i];
		}
	}
}

