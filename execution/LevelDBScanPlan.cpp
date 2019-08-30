#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "common/ParseNodeVisitor.h"
#include "execution/ParseTools.h"
#include "execution/DBDataTypeHandler.h"
#include "execution/LevelDBHandler.h"

ScanRange::ScanRange(const TableInfo* pTable, const ParseNode* pNode, ExecutionBuffer* pBuffer)
	: m_predicates(pTable->getKeyCount() + 1, PredicateInfo{}), m_pTable(pTable) {
	ExprFlattenVisitor visitor(Operation::AND, std::bind(&ScanRange::visit, this, pNode));
	visitor.visit(pNode);

	bool keyInvalid = true;
	for(size_t i=0;i<m_predicates.size();++i) {
		if(!keyInvalid) {
			m_predicates[i].m_op = Operation::NONE;
			continue;
		}
		switch(m_predicates[i].m_op) {
		case Operation::COMP_EQ:
			break;
		case Operation::COMP_GE:
		case Operation::COMP_GT:
			//ex. m_predicates[i] is key > x and m_predicates[i + 1] is key < y
			if(i + 1 < m_predicates.size() && m_predicates[i + 1].isPrevEndRange()) {
				break;
			}
			keyInvalid = false;
			break;
		case Operation::COMP_LE:
		case Operation::COMP_LT:
			assert(i > 0);
			if(m_predicates[i - 1].m_op == Operation::COMP_EQ) {
				//ex. m_predicates[i - 1] is key == x and m_predicates[i] is key < y
				//ignore end range since
				m_predicates[i].m_op = Operation::NONE;
			}
			keyInvalid = false;
			break;
		case Operation::NONE:
			keyInvalid = true;
			break;
		default:
			assert(0);
		}
	}
	size_t keyCount = pTable->getKeyCount();
	std::vector<ExecutionResult> startKeyResults(keyCount);
	std::vector<ExecutionResult> endKeyResults(keyCount);
	std::vector<DBDataType> keyTypes(keyCount);

	bool nextStartMax = false;
	for(size_t i=0;i<keyCount;++i) {
		auto pColumn = m_pTable->getColumn(i);
		keyTypes[i] = pColumn->m_type;

		auto pHandler = DBDataTypeHandler::getHandler(keyTypes[i]);
		auto& info = m_predicates[i];

		switch(info.m_op) {
		case Operation::COMP_EQ:
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			endKeyResults[i] = startKeyResults[i];
			continue;
		case Operation::COMP_GE:
			//same with equals
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			break;
		case Operation::COMP_GT:
			//key columns has larger index should be set to
			//maximum value to exclude all possible keys have same ith key column value
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			nextStartMax = true;
			break;
		default:
			if(nextStartMax) {
				pHandler->setToMax(startKeyResults[i]);
			} else {
				pHandler->setToMin(startKeyResults[i]);
			}
			break;
		}
		if(m_predicates[i + 1].isPrevEndRange()) {
			pHandler->fromNode(m_predicates[i + 1].m_pValue, endKeyResults[i]);
		} else {
			pHandler->setToMax(endKeyResults[i]);
		}
	}
	m_sStartRow = pBuffer->copyRow(startKeyResults, keyTypes);

	m_sEndRow = pBuffer->copyRow(endKeyResults, keyTypes);
}
void ScanRange::visit(const ParseNode* pPredicate) {
	if (pPredicate->m_type != NodeType::OP) {
		return;
	}

	auto op = pPredicate->m_op;
	assert(op != Operation::OR);

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
		return;
	}

	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);

	if(pLeft->m_type == NodeType::NAME) {
		setPredicateInfo(op, pLeft, pRight, pPredicate);
	} else if (pRight->m_type == NodeType::NAME) {
		setPredicateInfo(reverseOp, pRight, pLeft, pPredicate);
	} else {
		return;
	}
}

void ScanRange::setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate) {
	if(!pValue->isConst()) {
		return;
	}
	auto pKeyColumn = m_pTable->getColumnByName(pKey->m_sValue);
	if(pKeyColumn->m_iKeyIndex < 0) {
		return;
	}

	assert(pKeyColumn->m_iKeyIndex + 1 < m_predicates.size());

	//it is ok to ignore some predicates, since we have FilterPlan to check all predicates
	PredicateInfo* pInfo;
	switch(op) {
	case Operation::COMP_EQ:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_LE || pInfo->m_op == Operation::COMP_LT) {
			// <,<= for index - 1 is set to index, it will overwrite = at index
			// since it make any key search at index invalid
			return;
		}
		break;
	case Operation::COMP_GT:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_GE) {
			//> can overwrite >=, since it has smaller scan range
			break;
		}else if(pInfo->m_op != Operation::NONE) {
			return;
		}
		break;
	case Operation::COMP_GE:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op != Operation::NONE) {
			return;
		}
		break;
	case Operation::COMP_LE:
		// <= at index is set to index + 1, this will overwrite >, >=,= at index + 1
		// since <= at index make any key search at index + 1 invalid
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex + 1;
		if(pInfo->m_op == Operation::COMP_LT) {
			//<= can not overwrite <, since it has larger scan range
			return;
		}
		break;
	case Operation::COMP_LT:
		// < at index is set to index + 1, this will overwrite >, >=,= at index + 1
		// since < at index make any key search at index + 1 invalid
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex + 1;
		break;
	default:
		return;
	}
	pInfo->m_pKeyColumn = pKeyColumn;
	pInfo->m_pValue = pValue;
	pInfo->m_op = op;
	pInfo->m_sExpr = pPredicate->m_sExpr;
}
constexpr size_t SCAN_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable)
	: LeafPlan(PlanType::Scan), m_pTable(pTable){
	m_pBuffer = std::make_unique<ExecutionBuffer>(SCAN_BUFFER_SIZE);
}

int LevelDBScanPlan::addProjection(const ParseNode* pColumn) {
	return 0;
}

void LevelDBScanPlan::begin() {
	m_iRows = 0;
	m_pDBIter.reset(LevelDBHandler::getHandler(m_pTable)->createIterator());


}
bool LevelDBScanPlan::next() {
	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {
	m_pDBIter = nullptr;
}

bool LevelDBScanPlan::addPredicate(const ParseNode* pPredicate) {
	if(!m_scanRanges.empty() && m_scanRanges.front()->isFullScan() ){
		return false;
	}
	auto pRange = std::make_shared<ScanRange>(m_pTable, pPredicate, m_pBuffer.get());
	if(pRange->isFullScan()){
		m_scanRanges.clear();
		m_scanRanges.push_back(pRange);
		return false;
	}
	m_scanRanges.push_back(pRange);
	return true;
}
