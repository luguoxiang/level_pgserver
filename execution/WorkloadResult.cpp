#include "WorkloadResult.h"
#include "execution/ParseTools.h"
#include <sstream>
namespace {
std::vector<const char*> WorkloadColumns = { "TID", "Running", "Session", "ObExec",
		"BiggestExec", "SessionTime", "SqlCount" };
}

void WorkloadResult::getAllColumns(std::vector<std::string>& columns) {
	size_t count = sizeof(WorkloadColumns) / sizeof(const char*);
	for (auto& column : WorkloadColumns) {
		columns.push_back(column);
	}
}

int WorkloadResult::addProjection(ParseNode* pNode) {
	assert(pNode);
	size_t count = sizeof(WorkloadColumns) / sizeof(const char*);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < count; ++i) {
		if (Tools::case_equals(WorkloadColumns[i], pNode->m_sValue)) {
			return i;
		}
	}
	return -1;
}

std::string WorkloadResult::getInfoString() {
	return ConcateToString("SELECT ", WorkerManager::getInstance().getWorkerCount());
}

void WorkloadResult::begin() {
	m_iIndex = 0;
}

bool WorkloadResult::next() {
	++m_iIndex;
	return m_iIndex <= WorkerManager::getInstance().getWorkerCount();
}

std::string WorkloadResult::getProjectionName(size_t index) {
	return WorkloadColumns[index];
}

DBDataType WorkloadResult::getResultType(size_t index) {
	if (index == 0) {
		return DBDataType::STRING;
	}
	return DBDataType::INT32;
}

void WorkloadResult::getResult(size_t index, ExecutionResult* pInfo) {
	WorkThreadInfo* pWorker = WorkerManager::getInstance().getWorker(
			m_iIndex - 1);

	switch (index) {
	case 0: {
		std::stringstream ss;
		ss << pWorker->m_tid;
		m_tid = ss.str();
		pInfo->setStringView(m_tid);
		break;
	}
	case 1:
		pInfo->setInt(pWorker->m_bRunning);
		break;
	case 2:
		pInfo->setInt(pWorker->m_iSessions);
		break;
	case 3:
		pInfo->setInt(pWorker->m_iExecScanTime / 1000);
		break;
	case 4:
		pInfo->setInt(pWorker->m_iBiggestExec / 1000);
		break;
	case 5:
		pInfo->setInt(pWorker->m_iClientTime / 1000);
		break;
	case 6:
		pInfo->setInt(pWorker->m_iSqlCount);
		break;
	default:
		assert(0);
		break;
	};
}
