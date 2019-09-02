#include "LevelDBInsertPlan.h"
#include "ExecutionException.h"

#include <vector>
#include <set>

constexpr size_t INSERT_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBInsertPlan::LevelDBInsertPlan(const TableInfo* pTable, ExecutionPlan* pPlan)
	: SingleChildPlan(PlanType::Insert, std::move(pPlan)), m_pTable(pTable) {
	m_pBuffer = std::make_unique<ExecutionBuffer>(INSERT_BUFFER_SIZE);
}

void LevelDBInsertPlan::begin() {

	if (m_pPlan->getResultColumns() != m_pTable->getColumnCount()) {
		EXECUTION_ERROR("the numbers of insert columns and values are not matched");
	}

	m_iInsertRows = 0;

	m_pPlan->begin();
}
bool LevelDBInsertPlan::next() {
	bool hasMore = m_pPlan->next();
	if(!hasMore) {
		return false;
	}

	m_pBuffer->purge();

	std::vector<ExecutionResult> keyResults;
	std::vector<ExecutionResult> valueResults;
	std::vector<DBDataType> keyTypes;
	std::vector<DBDataType> valueTypes;

	for(size_t i=0;i<m_pTable->getKeyCount();++i) {
		auto pColumn = m_pTable->getKeyColumn(i);

		ExecutionResult result;
		m_pPlan->getResult(pColumn->m_iIndex, result);

		keyResults.push_back(result);
		keyTypes.push_back(pColumn->m_type);
	}

	for(size_t i=0;i<m_pTable->getColumnCount();++i) {
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex>= 0) {
			continue;
		}
		ExecutionResult result;
		m_pPlan->getResult(i, result);

		valueResults.push_back(result);
		valueTypes.push_back(pColumn->m_type);
	}


	auto keyRow = m_pBuffer->copyRow(keyResults, keyTypes);
	auto valueRow = m_pBuffer->copyRow(valueResults, valueTypes);
	m_batch.insert(keyRow, valueRow);
	++m_iInsertRows;
	return true;
}
void LevelDBInsertPlan::end() {
	m_pPlan->end();
	LevelDBHandler::getHandler(m_pTable)->commit(m_batch);
}
