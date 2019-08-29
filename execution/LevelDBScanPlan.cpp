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

bool LevelDBScanPlan::addPredicate(const ParseNode* pPredicate, std::vector<const ParseNode*>& unsolved) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		unsolved.push_back(pPredicate);
		return false;
	}

	auto op = pPredicate->m_op;
	if (op == Operation::AND) {
		bool valid = false;
		for (size_t i=0;i<pPredicate->children(); ++i ) {
			valid = addPredicate(pPredicate->getChild(i), unsolved) || valid;
		}
		return valid;
	}
	assert(op != Operation::OR);

	if(!addSimplePredicate(pPredicate)) {
		unsolved.push_back(pPredicate);
		return false;
	}
	return true;
}

bool LevelDBScanPlan::addSimplePredicate(const ParseNode* pPredicate) {
	if (pPredicate->children() != 2) {
		return false;
	}
	auto op = pPredicate->m_op;
	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);

	const DBColumnInfo* pKeyColumn = nullptr;
	const ParseNode* pValue = nullptr;
	if(pLeft->m_type == NodeType::NAME) {
		pKeyColumn = m_pTable->getColumnByName(pLeft->m_sValue);
		pValue = pRight;
		switch (op) {
		case Operation::COMP_EQ:
		case Operation::COMP_LE:
		case Operation::COMP_LT:
		case Operation::COMP_GT:
		case Operation::COMP_GE:
			break;
		default:
			return false;
		}
	} else if (pRight->m_type == NodeType::NAME) {
		pKeyColumn = m_pTable->getColumnByName(pRight->m_sValue);
		pValue = pLeft;
		switch (op) {
		case Operation::COMP_EQ:
			break;
		case Operation::COMP_LE:
			op = Operation::COMP_GE;
			break;
		case Operation::COMP_LT:
			op = Operation::COMP_GT;
			break;
		case Operation::COMP_GT:
			op = Operation::COMP_LT;
			break;
		case Operation::COMP_GE:
			op = Operation::COMP_LE;
			break;
		default:
			return false;
		}
	} else {
		return false;
	}
	if(pKeyColumn->m_iKeyIndex < 0 || !pValue->isConst()) {
		return false;
	}

	return true;
}
