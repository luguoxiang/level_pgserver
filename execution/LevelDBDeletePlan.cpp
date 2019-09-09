#include "LevelDBDeletePlan.h"
#include "ExecutionException.h"
#include "common/ParseTools.h"
#include <vector>
#include <set>


LevelDBDeletePlan::LevelDBDeletePlan(const TableInfo* pTable, ExecutionPlan* pPlan)
	: SingleChildPlan(PlanType::Delete, pPlan), m_pTable(pTable) {

}

void LevelDBDeletePlan::begin() {

	std::string_view name(Tools::ROWKEY);
	ParseNode rowkey(NodeType::NAME, Operation::NONE, name, 0, nullptr);
	rowkey.setString(name);

	m_iRowkeyProjection = m_pPlan->addProjection(&rowkey);
	assert (m_iRowkeyProjection >=0);

	m_iDeleteRows = 0;

	m_pPlan->begin();
}
bool LevelDBDeletePlan::next() {
	if(!m_pPlan->next()) {
		return false;
	}

	m_pPlan->getResult(m_iRowkeyProjection, m_rowkey, DBDataType::BYTES);
	m_batch.remove(m_rowkey.getString());

	++m_iDeleteRows;
	return true;
}
void LevelDBDeletePlan::end() {
	m_pPlan->end();
	LevelDBHandler::getHandler(m_pTable)->commit(m_batch);
}
