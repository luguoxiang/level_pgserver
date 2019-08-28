#include "LevelDBScanPlan.h"
#include "ExecutionException.h"

constexpr size_t SCAN_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable, ExecutionPlan* pPlan)
	: SingleChildPlan(PlanType::Scan, pPlan), m_pTable(pTable) {
	m_pBuffer = std::make_unique<ExecutionBuffer>(SCAN_BUFFER_SIZE);
}

void LevelDBScanPlan::begin() {



	m_iRows = 0;

	m_pPlan->begin();
}
bool LevelDBScanPlan::next() {
	bool hasMore = m_pPlan->next();
	if(!hasMore) {
		return false;
	}


	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {
	m_pPlan->end();
}
