#include "LevelDBInsertPlan.h"
#include "ExecutionException.h"

#include <vector>
#include <set>


LevelDBInsertPlan::LevelDBInsertPlan(const TableInfo* pTable, ExecutionPlan* pPlan)
	: SingleChildPlan(PlanType::Insert, std::move(pPlan)), m_pTable(pTable) {
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

	DataRow row(keyTypes);

	size_t rowSize = row.computeSize(keyResults);
	m_keyBuffer.clear();
	m_keyBuffer.resize(rowSize);
	row.copy(keyResults, m_keyBuffer.data());

	rowSize = row.computeSize(valueResults);
	m_valueBuffer.clear();
	m_valueBuffer.resize(rowSize);
	row.copy(valueResults, m_valueBuffer.data());

	m_batch.insert(m_keyBuffer, m_valueBuffer);
	++m_iInsertRows;
	return true;
}
void LevelDBInsertPlan::end() {
	m_pPlan->end();
	LevelDBHandler::getHandler(m_pTable)->commit(m_batch);
}
