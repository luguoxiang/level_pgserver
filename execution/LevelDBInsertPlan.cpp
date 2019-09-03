#include "LevelDBInsertPlan.h"
#include "ExecutionException.h"

#include <vector>
#include <set>


LevelDBInsertPlan::LevelDBInsertPlan(const TableInfo* pTable, ExecutionPlan* pPlan)
	: SingleChildPlan(PlanType::Insert, std::move(pPlan)), m_pTable(pTable) {
	for(size_t i=0;i<m_pTable->getColumnCount();++i) {
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex>= 0) {
			m_keyTypes.push_back(pColumn->m_type);
		} else {
			m_valueTypes.push_back(pColumn->m_type);
		}
	}
	m_keyResults.reserve(m_keyTypes.size());
	m_valueResults.reserve(m_valueTypes.size());
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

	for(size_t i=0;i<m_pTable->getColumnCount();++i) {
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex>= 0) {
			m_pPlan->getResult(i, m_keyResults[pColumn->m_iKeyIndex]);
		} else {
			assert(pColumn->m_iValueIndex >= 0);
			m_pPlan->getResult(i, m_valueResults[pColumn->m_iValueIndex]);
		}
	}

	DataRow keyRow(m_keyTypes);

	size_t rowSize = keyRow.computeSize(m_keyResults);
	m_keyBuffer.clear();
	m_keyBuffer.resize(rowSize);
	keyRow.copy(m_keyResults, m_keyBuffer.data());

	DataRow valueRow(m_valueTypes);
	rowSize = valueRow.computeSize(m_valueResults);
	m_valueBuffer.clear();
	m_valueBuffer.resize(rowSize);
	valueRow.copy(m_valueResults, m_valueBuffer.data());

	m_batch.insert(m_keyBuffer, m_valueBuffer);
	++m_iInsertRows;
	return true;
}
void LevelDBInsertPlan::end() {
	m_pPlan->end();
	LevelDBHandler::getHandler(m_pTable)->commit(m_batch);
}
