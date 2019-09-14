#include "WorkerManager.h"

void WorkerManager::cancel(bool planOnly) {
	for(auto& pWorker : m_workers) {
		pWorker->cancel(planOnly);
	}
}
