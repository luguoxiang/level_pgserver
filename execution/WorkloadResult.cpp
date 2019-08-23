#include "WorkloadResult.h"
#include "execution/ParseTools.h"
#include <sstream>
namespace {
std::vector<const char*> WorkloadColumns = { "TID", "Running", "Session", "ObExec",
		"BiggestExec", "SessionTime", "SqlCount" };
}

void WorkloadResult::getAllColumns(std::vector<std::string_view>& columns) {
	for (auto& column : WorkloadColumns) {
		columns.push_back(column);
	}
}

int WorkloadResult::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < WorkloadColumns.size(); ++i) {
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

std::string_view WorkloadResult::getProjectionName(size_t index) {
	return WorkloadColumns[index];
}

DBDataType WorkloadResult::getResultType(size_t index) {
	return DBDataType::INT32;
}

void WorkloadResult::getResult(size_t index, ExecutionResult* pInfo) {
	WorkThreadInfo* pWorker = WorkerManager::getInstance().getWorker(
			m_iIndex - 1);

	switch (index) {
	case 0: {
		pInfo->setInt(pWorker->m_iIndex);
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
