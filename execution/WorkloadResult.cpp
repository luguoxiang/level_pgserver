#include "WorkloadResult.h"
#include <sstream>
namespace {
const char* WorkloadColumns[] = { "TID", "Running", "Session", "ObExec",
		"BiggestExec", "SessionTime", "SqlCount" };
}

void WorkloadResult::getAllColumns(std::vector<const char*>& columns) {
	size_t count = sizeof(WorkloadColumns) / sizeof(const char*);
	for (int i = 0; i < count; ++i) {
		columns.push_back(WorkloadColumns[i]);
	}
}

int WorkloadResult::addProjection(ParseNode* pNode) {
	assert(pNode);
	size_t count = sizeof(WorkloadColumns) / sizeof(const char*);
	if (pNode->m_iType != NAME_NODE)
		return -1;
	for (size_t i = 0; i < count; ++i) {
		if (strcasecmp(WorkloadColumns[i], pNode->m_pszValue) == 0) {
			return i;
		}
	}
	return -1;
}

void WorkloadResult::getInfoString(char* szBuf, int len) {
	snprintf(szBuf, len, "SELECT %lu",
			WorkerManager::getInstance().getWorkerCount());
}

void WorkloadResult::begin() {
	m_iIndex = 0;
}

bool WorkloadResult::next() {
	++m_iIndex;
	return m_iIndex <= WorkerManager::getInstance().getWorkerCount();
}

const char* WorkloadResult::getProjectionName(size_t index) {
	return WorkloadColumns[index];
}

DBDataType WorkloadResult::getResultType(size_t index) {
	if (index == 0) {
		return TYPE_STRING;
	}
	return TYPE_INT32;
}

void WorkloadResult::getResult(size_t index, ResultInfo* pInfo) {
	WorkThreadInfo* pWorker = WorkerManager::getInstance().getWorker(
			m_iIndex - 1);

	pInfo->m_bNull = false;
	switch (index) {
	case 0: {
		std::stringstream ss;
		ss << pWorker->m_tid;
		m_tid = ss.str();
		pInfo->m_value.m_pszResult = m_tid.c_str();
		break;
	}
	case 1:
		pInfo->m_value.m_lResult = pWorker->m_bRunning;
		break;
	case 2:
		pInfo->m_value.m_lResult = pWorker->m_iSessions;
		break;
	case 3:
		pInfo->m_value.m_lResult = pWorker->m_iExecScanTime / 1000;
		break;
	case 4:
		pInfo->m_value.m_lResult = pWorker->m_iBiggestExec / 1000;
		break;
	case 5:
		pInfo->m_value.m_lResult = pWorker->m_iClientTime / 1000;
		break;
	case 6:
		pInfo->m_value.m_lResult = pWorker->m_iSqlCount;
		break;
	default:
		assert(0);
		break;
	};
}
