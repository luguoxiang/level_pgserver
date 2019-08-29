#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "execution/ParseTools.h"
#include "execution/DBDataTypeHandler.h"
#include "execution/LevelDBHandler.h"

constexpr size_t SCAN_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable)
	: LeafPlan(PlanType::Scan), m_pTable(pTable),
	  m_predicates(pTable->getKeyCount(), PredicateInfo{}){
	m_pBuffer = std::make_unique<ExecutionBuffer>(SCAN_BUFFER_SIZE);
}

void LevelDBScanPlan::begin() {
	m_iRows = 0;

	size_t keyCount = m_pTable->getKeyCount();
	std::vector<ExecutionResult> startKeyResults(keyCount);
	std::vector<DBDataType> keyTypes(keyCount);

	assert(m_predicates.size() == keyCount);

	bool setMin = false, setMax = false;
	for(size_t i=0;i<keyCount;++i) {
		auto pColumn = m_pTable->getColumn(i);
		keyTypes[i] = pColumn->m_type;

		auto pHandler = DBDataTypeHandler::getHandler(keyTypes[i]);
		auto& info = m_predicates[i];

		if(info.m_startOp == Operation::COMP_EQ) {
			assert(info.m_endOp == Operation::COMP_EQ);
			//set ith key column to m_pStart, key columns has larger index should be set to
			//minimum value to include all possible keys have same ith key column value
			pHandler->fromNode(info.m_pStart, startKeyResults[i]);
			setMin = true;
			continue;
		}
		if(info.m_startOp == Operation::COMP_GE) {
			//same with equals
			pHandler->fromNode(info.m_pStart, startKeyResults[i]);
			setMin = true;
			continue;
		} else if(info.m_startOp == Operation::COMP_GT) {
			//set ith key column to m_pStart, key columns has larger index should be set to
			//maximum value to exclude all possible keys have same ith key column value
			pHandler->fromNode(info.m_pStart, startKeyResults[i]);
			setMax = true;
			continue;
		}

		if(info.m_endOp ==  Operation::COMP_LE || info.m_endOp == Operation::COMP_LT) {
			//should set current key column to minimum value to include all possible keys
			setMin = true;
			break;
		}
		if(setMin) {
			assert(!setMax);
			pHandler->setToMin(startKeyResults[i]);
		}
		if(setMax) {
			assert(!setMin);
			pHandler->setToMax(startKeyResults[i]);
		}
	}
	auto [keyRow, iKeySize] = m_pBuffer->copyRow(startKeyResults, keyTypes);
	m_pDBIter->seek(std::string_view(reinterpret_cast<const char*>(keyRow), iKeySize));
}
bool LevelDBScanPlan::next() {
	if(m_iRows > 0) {
		m_pDBIter->next();
	}
	if(!m_pDBIter->valid()) {
		return false;
	}
	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {
	m_pDBIter = nullptr;
}

bool LevelDBScanPlan::addPredicate(const ParseNode* pPredicate, std::vector<const ParseNode*>& unsolved) {
	doAddPredicate(pPredicate, unsolved);
	if(m_predicates[0].m_startOp == Operation::NONE && m_predicates[0].m_endOp == Operation::NONE) {
		return false;
	}
	bool keyValid = true;
	for(size_t i=0;i<m_predicates.size();++i) {
		auto& info = m_predicates[i];
		if(!keyValid) {
			info.m_startOp = Operation::NONE;
			info.m_endOp = Operation::NONE;
			if(info.m_pStartExpr != nullptr) {
				unsolved.push_back(info.m_pStartExpr);
			}
			if(info.m_pEndExpr != nullptr && info.m_pEndExpr != info.m_pStartExpr) {
				unsolved.push_back(info.m_pEndExpr);
			}
			continue;
		}
		if(info.m_startOp == Operation::COMP_EQ) {
			assert(info.m_endOp == Operation::COMP_EQ && info.m_pStart != nullptr && info.m_pStart == info.m_pEnd);
		} else {
			keyValid = false;
		}
	}
	return true;
}
void LevelDBScanPlan::doAddPredicate(const ParseNode* pPredicate, std::vector<const ParseNode*>& unsolved) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		unsolved.push_back(pPredicate);
		return;
	}

	auto op = pPredicate->m_op;
	if (op == Operation::AND) {
		for (size_t i=0;i<pPredicate->children(); ++i ) {
			doAddPredicate(pPredicate->getChild(i), unsolved);
		}
		return;
	}
	assert(op != Operation::OR);

	if(!addSimplePredicate(pPredicate)) {
		unsolved.push_back(pPredicate);
	}
}

bool LevelDBScanPlan::addSimplePredicate(const ParseNode* pPredicate) {
	if (pPredicate->children() != 2) {
		return false;
	}
	auto op = pPredicate->m_op;
	Operation reverseOp;
	switch (op) {
	case Operation::COMP_EQ:
		reverseOp = op;
		break;
	case Operation::COMP_LE:
		reverseOp = Operation::COMP_GE;
		break;
	case Operation::COMP_LT:
		reverseOp = Operation::COMP_GT;
		break;
	case Operation::COMP_GT:
		reverseOp = Operation::COMP_LT;
		break;
	case Operation::COMP_GE:
		reverseOp = Operation::COMP_LE;
		break;
	default:
		return false;
	}

	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);

	const DBColumnInfo* pKeyColumn = nullptr;
	const ParseNode* pValue = nullptr;
	if(pLeft->m_type == NodeType::NAME) {
		pKeyColumn = m_pTable->getColumnByName(pLeft->m_sValue);
		pValue = pRight;
	} else if (pRight->m_type == NodeType::NAME) {
		pKeyColumn = m_pTable->getColumnByName(pRight->m_sValue);
		pValue = pLeft;
		op = reverseOp;
	} else {
		return false;
	}
	if(pKeyColumn->m_iKeyIndex < 0 || !pValue->isConst()) {
		return false;
	}
	assert(pKeyColumn->m_iKeyIndex < m_predicates.size());
	auto& predicte= m_predicates[pKeyColumn->m_iKeyIndex];

	if(op == Operation::COMP_EQ || op == Operation::COMP_LE || op == Operation::COMP_LT) {
		if(predicte.m_pEnd != nullptr) {
			assert(predicte.m_pEndExpr);
			PARSE_ERROR("Duplicate or conflict predicates on ", pKeyColumn->m_name, ", operator:", predicte.m_pEndExpr->m_sExpr, ",", pPredicate->m_sExpr);
		}
		predicte.m_pEnd = pValue;
		predicte.m_pEndExpr = pPredicate;
		predicte.m_endOp = op;
	}
	if(op == Operation::COMP_EQ || op == Operation::COMP_GT || op == Operation::COMP_GE) {
		if(predicte.m_pStart != nullptr) {
			assert(predicte.m_pStartExpr);
			PARSE_ERROR("Duplicate or conflict predicates on ", pKeyColumn->m_name, ", operator:", predicte.m_pStartExpr->m_sExpr, ",", pPredicate->m_sExpr);
		}
		predicte.m_pStart = pValue;
		predicte.m_pStartExpr = pPredicate;
		predicte.m_startOp = op;
	}

	return true;
}
