#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "execution/ParseTools.h"
#include "execution/DBDataTypeHandler.h"
#include "execution/LevelDBHandler.h"
#include "execution/DataRow.h"


void LevelDBScanPlan::setPredicate(const ParseNode* pNode, std::vector<const ParseNode*>& unsolved) {
	doSetPredicate(pNode, unsolved);

	bool keyInvalid = true;
	m_bStartInclusive = true;
	m_bEndInclusive = true;

	for(size_t i=0;i<m_predicates.size();++i) {
		if(!keyInvalid) {
			m_predicates[i].m_op = Operation::NONE;
		}
		auto op = m_predicates[i].m_op;

		switch(op) {
		case Operation::COMP_EQ:
			break;
		case Operation::COMP_GT:
			m_bEndInclusive = false;
			//fall throught
		case Operation::COMP_GE:
			//ex. m_predicates[i] is key > x and m_predicates[i + 1] is key < y
			if(i + 1 < m_predicates.size() && m_predicates[i + 1].isPrevEndRange()) {
				break;
			}
			keyInvalid = false;
			break;

		case Operation::COMP_LT:
			m_bEndInclusive = false;
			//fall throught
		case Operation::COMP_LE:

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
	size_t keyCount = m_pTable->getKeyCount();
	std::vector<ExecutionResult> startKeyResults(keyCount);
	std::vector<ExecutionResult> endKeyResults(keyCount);

	bool nextStartMax = false;
	for(size_t i=0;i<keyCount;++i) {
		auto pColumn = m_pTable->getKeyColumn(i);

		auto pHandler = DBDataTypeHandler::getHandler(m_keyTypes[i]);
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
	m_startRow = m_pBuffer->copyRow(startKeyResults, m_keyTypes);

	m_endRow = m_pBuffer->copyRow(endKeyResults, m_keyTypes);
}

void LevelDBScanPlan::doSetPredicate(const ParseNode* pPredicate, std::vector<const ParseNode*>& unsolved) {
	if (pPredicate->m_type != NodeType::OP) {
		unsolved.push_back(pPredicate);
		return;
	}

	auto op = pPredicate->m_op;

	Operation reverseOp;
	switch (op) {
	case Operation::OR:
		assert(0);
		return;
	case Operation::AND:
		for(size_t i=0;i<pPredicate->children();++i) {
			doSetPredicate(pPredicate->getChild(i), unsolved);
		}
		return;
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
		unsolved.push_back(pPredicate);
		return;
	}

	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);

	bool solved = false;
	if(pLeft->m_type == NodeType::NAME) {
		solved = setPredicateInfo(op, pLeft, pRight, pPredicate);
	} else if (pRight->m_type == NodeType::NAME) {
		solved = setPredicateInfo(reverseOp, pRight, pLeft, pPredicate);
	}
	if(!solved) {
		unsolved.push_back(pPredicate);
	}
}

bool LevelDBScanPlan::setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate) {
	if(!pValue->isConst()) {
		return false;
	}
	auto pKeyColumn = m_pTable->getColumnByName(pKey->m_sValue);
	if(pKeyColumn->m_iKeyIndex < 0) {
		return false;
	}

	assert(pKeyColumn->m_iKeyIndex + 1 < m_predicates.size());

	KeyPredicateInfo* pInfo;
	switch(op) {
	case Operation::COMP_EQ:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_LE || pInfo->m_op == Operation::COMP_LT) {
			// <,<= for index - 1 is set to index, it will overwrite = at index
			// since it make any key search at index invalid
			return false;
		}
		break;
	case Operation::COMP_GT:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_GE) {
			//> can overwrite >=, since it has smaller scan range
			break;
		}else if(pInfo->m_op != Operation::NONE) {
			return false;
		}
		break;
	case Operation::COMP_GE:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op != Operation::NONE) {
			return false;
		}
		break;
	case Operation::COMP_LE:
		// <= at index is set to index + 1, this will overwrite >, >=,= at index + 1
		// since <= at index make any key search at index + 1 invalid
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex + 1;
		if(pInfo->m_op == Operation::COMP_LT) {
			//<= can not overwrite <, since it has larger scan range
			return false;
		}
		break;
	case Operation::COMP_LT:
		// < at index is set to index + 1, this will overwrite >, >=,= at index + 1
		// since < at index make any key search at index + 1 invalid
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex + 1;
		break;
	default:
		return false;
	}
	pInfo->m_pKeyColumn = pKeyColumn;
	pInfo->m_pValue = pValue;
	pInfo->m_op = op;
	pInfo->m_sExpr = pPredicate->m_sExpr;
	return true;
}
constexpr size_t SCAN_BUFFER_SIZE = 1 * 1024 * 1024;

LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable)
	: LeafPlan(PlanType::Scan), m_pTable(pTable)
	, m_columnValues(pTable->getColumnCount())
	, m_projection(pTable->getColumnCount())
	, m_currentRow(m_keyTypes)
	, m_predicates(pTable->getKeyCount() + 1, KeyPredicateInfo{})
	, m_startRow(m_keyTypes)
	, m_endRow(m_keyTypes)
	, m_keyTypes(pTable->getKeyCount()) {

	m_pBuffer = std::make_unique<ExecutionBuffer>(SCAN_BUFFER_SIZE);

	for(size_t i = 0;i<pTable->getColumnCount();++i) {
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex < 0) {
			m_valueTypes.push_back(pColumn->m_type);
		} else {
			m_keyTypes[pColumn->m_iKeyIndex] = pColumn->m_type;
		}
	}
}

int LevelDBScanPlan::addProjection(const ParseNode* pNode) {
	if(pNode->m_type == NodeType::NAME) {
		if(pNode->m_sValue == "_rowkey") {
			return m_columnValues.size();
		}
		auto pColumn = m_pTable->getColumnByName(pNode->m_sValue);
		m_projection[pColumn->m_iIndex] = true;
		return pColumn->m_iIndex;
	}
	return -1;
}

void LevelDBScanPlan::begin() {
	m_iRows = 0;
	m_pDBIter.reset(LevelDBHandler::getHandler(m_pTable)->createIterator());
	if(m_order ==SortOrder::Descend) {
		if(isSeekToLast()) {
			m_pDBIter->last();
			return;
		}
		m_pDBIter->seek(m_endRow);
		if(!m_pDBIter->valid()) {
			m_pDBIter->last();
			return;
		}
		if(!m_bEndInclusive) {
			for(;m_pDBIter->valid();m_pDBIter->prev()) {
				m_currentRow = m_pDBIter->key(m_keyTypes);
				int n = m_currentRow.compare(m_endRow);
				if(n < 0) {
					return;
				}
			}
		}
	} else {
		if(isSeekToFirst()) {
			m_pDBIter->first();
			return;
		}
		m_pDBIter->seek(m_startRow);

		if(!m_bStartInclusive) {
			for(;m_pDBIter->valid();m_pDBIter->next()) {
				m_currentRow = m_pDBIter->key(m_keyTypes);
				int n = m_currentRow.compare(m_startRow);
				if(n > 0) {
					return;
				}
			}
		}
	}

}
bool LevelDBScanPlan::next() {
	if(!m_pDBIter->valid()) {
		return false;
	}
	m_currentRow = m_pDBIter->key(m_keyTypes);

	if(m_order ==SortOrder::Descend) {
		if(!isSeekToFirst()) {
			int n = m_currentRow.compare(m_startRow);
			if (n == 0 && !m_bStartInclusive) {
				return false;
			} else if(n < 0) {
				return false;
			}
		}
	} else if(!isSeekToLast() ) {
		int n = m_currentRow.compare(m_endRow);
		if (n == 0 && !m_bEndInclusive) {
			return false;
		} else if(n > 0) {
			return false;
		}
	}

	bool needValue = false;

	for(size_t i = 0;i<m_projection.size();++i) {
		if(!m_projection[i]){
			continue;
		}
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex >= 0) {
			m_currentRow.getResult(pColumn->m_iKeyIndex, m_columnValues[i]);
		} else {
			needValue =true;
		}

	}
	if(needValue) {
		size_t iValueIndex = 0;
		auto valueRow = m_pDBIter->value(m_valueTypes);
		for(size_t i = 0;i<m_projection.size();++i) {
			auto pColumn = m_pTable->getColumn(i);
			if(pColumn->m_iKeyIndex >= 0) {
				continue;
			} else {
				++iValueIndex;
			}
			if(m_projection[i]) {
				valueRow.getResult(iValueIndex - 1, m_columnValues[i]);
			}
		}
	}


	m_pDBIter->next();
	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {
	m_pDBIter = nullptr;
}

uint64_t LevelDBScanPlan::getCost() {
	return LevelDBHandler::getHandler(m_pTable)->getCost(m_startRow, m_endRow);
}

bool LevelDBScanPlan::ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
		SortOrder order) {
	if ( iSortIndex > m_pTable->getColumnCount()) {
		return false;
	}
	auto pColumn = m_pTable->getColumnByName(sColumn);
	if(pColumn->m_iKeyIndex != iSortIndex) {
		return false;
	}

	if (order == SortOrder::Any) {
		return true;
	}
	if(m_order == SortOrder::Any) {
		m_order = order;
	}
	return m_order == order;

}
