#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "execution/ParseTools.h"

constexpr size_t SCAN_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable)
	: LeafPlan(PlanType::Scan), m_pTable(pTable) {
	m_pBuffer = std::make_unique<ExecutionBuffer>(SCAN_BUFFER_SIZE);
}

void LevelDBScanPlan::begin() {



	m_iRows = 0;


}
bool LevelDBScanPlan::next() {



	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {

}

void LevelDBScanPlan::addPredicate(const ParseNode* pPredicate) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}

	auto op = pPredicate->m_op;
	if (op == Operation::AND) {
		for (size_t i=0;i<pPredicate->children(); ++i ) {
			addPredicate(pPredicate->getChild(i));
		}
		return;
	}
	assert(op != Operation::OR);
	if (pPredicate->children() != 2) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}


}
