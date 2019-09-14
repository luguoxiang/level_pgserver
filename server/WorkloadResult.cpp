#include <sstream>
#include <absl/strings/match.h>

#include "common/ParseTools.h"
#include "execution/ExecutionResult.h"

#include "WorkloadResult.h"
#include "WorkThreadInfo.h"
#include "WorkerManager.h"

namespace {
std::vector<const char*> WorkloadColumns = { "TID", "Running", "Session",  "SessionTime", "SqlCount" };
}

void WorkloadResult::getAllColumns(std::vector<std::string_view>& columns) {
	for (auto& column : WorkloadColumns) {
		columns.push_back(column);
	}
}
int WorkloadResult::getResultColumns() {
	return WorkloadColumns.size();
}
int WorkloadResult::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < WorkloadColumns.size(); ++i) {
		if (absl::EqualsIgnoreCase(WorkloadColumns[i], pNode->getString())) {
			return i;
		}
	}
	return -1;
}

std::string WorkloadResult::getInfoString() {
	return absl::StrCat("SELECT ", WorkerManager::getInstance().getWorkerCount());
}

void WorkloadResult::begin(const std::atomic_bool& bTerminated) {
	m_iIndex = 0;
}

bool WorkloadResult::next(const std::atomic_bool& bTerminated) {
	CheckCancellation(bTerminated);
	++m_iIndex;
	return m_iIndex <= WorkerManager::getInstance().getWorkerCount();
}

std::string_view WorkloadResult::getProjectionName(size_t index) {
	return WorkloadColumns[index];
}

DBDataType WorkloadResult::getResultType(size_t index) {
	switch (index) {
	case 0:
		return DBDataType::INT16;
	case 1:
		return DBDataType::BOOL;
	case 2:
	case 3:
	case 4:
		return DBDataType::INT64;
	default:
		assert(0);
		break;
	};
}

void WorkloadResult::getResult(size_t index, ExecutionResult& result, DBDataType type) {
	WorkThreadInfo* pWorker = WorkerManager::getInstance().getWorker(
			m_iIndex - 1);

	switch (index) {
	case 0:
		result.setInt(m_iIndex);
		break;
	case 1:
		result.setInt(pWorker->isRunning());
		break;
	case 2:
		result.setInt(pWorker->getSessions());
		break;
	case 3:
		result.setInt(pWorker->getClientTime());
		break;
	case 4:
		result.setInt(pWorker->getSqlCount());
		break;
	default:
		assert(0);
		break;
	};
}
