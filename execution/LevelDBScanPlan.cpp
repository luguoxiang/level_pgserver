#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "execution/ParseTools.h"
#include "execution/DBDataTypeHandler.h"
#include "execution/LevelDBHandler.h"
#include "execution/DataRow.h"


void LevelDBScanPlan::setPredicate(const ParseNode* pNode,std::set<std::string_view>& solved) {
	doSetPredicate(pNode);

	bool keyValid = true;
	m_bStartInclusive = true;
	m_bEndInclusive = true;

	for(size_t i=0;i<m_predicates.size();++i) {
		auto& info =m_predicates[i];
		if(!keyValid) {
			info.m_op = Operation::NONE;
		}
		switch(m_endPredicate.m_op) {
		case Operation::COMP_LT:
		case Operation::COMP_LE:
			if(i == m_endPredicate.m_iKeyIndex) {
				if(!keyValid || info.m_op == Operation::COMP_EQ) {
					m_endPredicate.m_op = Operation::NONE;
					m_endPredicate.m_iKeyIndex = -1;
				} else {
					m_bEndInclusive = (m_endPredicate.m_op == Operation::COMP_LE);
					keyValid = false;
					solved.insert(m_endPredicate.m_sExpr);
				}
			}
			break;
		default://NONE
			break;
		}
		switch(info.m_op) {
		case Operation::COMP_EQ:
			solved.insert(info.m_sExpr);
			break;
		case Operation::COMP_GT:
		case Operation::COMP_GE:
			m_bStartInclusive = (info.m_op == Operation::COMP_GE);
			keyValid = false;
			solved.insert(info.m_sExpr);
			break;
		case Operation::NONE:
			keyValid = false;
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
		assert(pHandler);
		auto& info = m_predicates[i];

		switch(info.m_op) {
		case Operation::COMP_EQ:
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			endKeyResults[i] = startKeyResults[i];
			DLOG(INFO) << "Key search " << info.m_sExpr << ", KeyIndex" << info.m_iKeyIndex;
			continue;
		case Operation::COMP_GE:
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			//key columns has larger index should be set to
			//minimum value to include all possible keys have same ith key column value
			nextStartMax = false;
			DLOG(INFO) << "Key search " << info.m_sExpr<< ", KeyIndex=" << info.m_iKeyIndex;
			break;
		case Operation::COMP_GT:
			pHandler->fromNode(info.m_pValue, startKeyResults[i]);
			//key columns has larger index should be set to
			//maximum value to exclude all possible keys have same ith key column value
			nextStartMax = true;
			DLOG(INFO) << "Key search " << info.m_sExpr<< ", KeyIndex=" << info.m_iKeyIndex;
			break;
		default:
			if(nextStartMax) {
				pHandler->setToMax(startKeyResults[i]);
			} else {
				pHandler->setToMin(startKeyResults[i]);
			}
			break;
		}
		if(i == m_endPredicate.m_iKeyIndex) {
			DLOG(INFO) << "Key search " << m_endPredicate.m_sExpr<< ", KeyIndex" << m_endPredicate.m_iKeyIndex;
			pHandler->fromNode(m_endPredicate.m_pValue, endKeyResults[i]);
		} else {
			pHandler->setToMax(endKeyResults[i]);
		}
	}
	m_startRow = m_pBuffer->copyRow(startKeyResults, m_keyTypes);

	m_endRow = m_pBuffer->copyRow(endKeyResults, m_keyTypes);
}

void LevelDBScanPlan::doSetPredicate(const ParseNode* pPredicate) {
	if (pPredicate->m_type != NodeType::OP) {
		return;
	}

	auto op = pPredicate->m_op;

	assert(op != Operation::OR);

	Operation reverseOp;
	switch (op) {
	case Operation::AND:
		for(size_t i=0;i<pPredicate->children();++i) {
			doSetPredicate(pPredicate->getChild(i));
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
		return;
	}

	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);

	if(pLeft->m_type == NodeType::NAME) {
		setPredicateInfo(op, pLeft, pRight, pPredicate);
	} else if (pRight->m_type == NodeType::NAME) {
		setPredicateInfo(reverseOp, pRight, pLeft, pPredicate);
	}
}

void LevelDBScanPlan::setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate) {
	if(!pValue->isConst()) {
		return;
	}
	auto pKeyColumn = m_pTable->getColumnByName(pKey->m_sValue);
	if(pKeyColumn->m_iKeyIndex < 0) {
		return;
	}

	assert(pKeyColumn->m_iKeyIndex + 1 < m_predicates.size());

	KeyPredicateInfo* pInfo;
	switch(op) {
	case Operation::COMP_EQ:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		break;
	case Operation::COMP_GT:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_EQ) {
			//= take precedence
			return;
		} else {
			break;
		}
	case Operation::COMP_GE:
		pInfo = m_predicates.data() + pKeyColumn->m_iKeyIndex;
		if(pInfo->m_op == Operation::COMP_EQ || pInfo->m_op == Operation::COMP_GT) {
			//= and > take precedence,  since it has smaller scan range
			return;
		}
		break;
	case Operation::COMP_LE:
		pInfo = &m_endPredicate;
		if(pInfo->m_op == Operation::COMP_LT) {
			//<= can not overwrite <, since it has larger scan range
			return;
		}
		break;
	case Operation::COMP_LT:
		pInfo = &m_endPredicate;
		break;
	default:
		assert(0);
		return;
	}
	pInfo->m_iKeyIndex = pKeyColumn->m_iKeyIndex;
	pInfo->m_pValue = pValue;
	pInfo->m_op = op;
	pInfo->m_sExpr = pPredicate->m_sExpr;
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
	if(Tools::isRowKeyNode(pNode)) {
		return m_columnValues.size();
	} else if(pNode->m_type == NodeType::NAME) {
		auto pColumn = m_pTable->getColumnByName(pNode->m_sValue);
		if(pColumn == nullptr ) {
			PARSE_ERROR("Unknown column: ", pNode->m_sValue);
		}

		m_projection[pColumn->m_iIndex] = true;
		return pColumn->m_iIndex;
	}
	return -1;
}

void LevelDBScanPlan::begin() {
	m_iRows = 0;

	if(m_pDBIter == nullptr) {
		m_pDBIter = LevelDBHandler::getHandler(m_pTable)->createIterator();
	}
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
void LevelDBScanPlan::explain(std::vector<std::string>& rows) {
	std::vector<ExecutionResult> start(m_keyTypes.size());
	std::vector<ExecutionResult> end(m_keyTypes.size());
	if(m_startRow.data() == nullptr) {
		for(size_t i=0;i<m_keyTypes.size();++i) {
			auto pHandler = DBDataTypeHandler::getHandler(m_keyTypes[i]);
			assert(pHandler);
			pHandler->setToMin(start[i]);
			pHandler->setToMax(end[i]);
		}
		m_startRow = m_pBuffer->copyRow(start, m_keyTypes);
		m_endRow = m_pBuffer->copyRow(end, m_keyTypes);
	} else {
		m_startRow.getResult(start);
		m_endRow.getResult(end);
	}

	assert(start.size() == end.size());
	std::string s = "leveldb:scan  ";
	s.append(m_pTable->getName());
	s.append("cost:");
	s.append(std::to_string(getCost()));
	rows.push_back(s);
	s = "  range:";
	s.append(m_bStartInclusive?"[":"(");
	for(auto& result: start) {
		s.append(result.toString());
		s.append("|");
	}
	s.erase(s.length() -1, 1);
	s.append(", ");
	for(auto& result: end) {
		s.append(result.toString());
		s.append("|");
	}
	s.erase(s.length() -1, 1);
	s.append(m_bEndInclusive?"]":")");
	rows.push_back(s);

}

void LevelDBScanPlan::getResult(size_t columnIndex, ExecutionResult& result) {
	if(m_columnValues.size() == columnIndex) {
		result.setStringView(std::string_view(m_currentRow.data(), m_currentRow.size()));
	}else {
		assert(columnIndex < m_columnValues.size());
		result = m_columnValues[columnIndex];
	}
}
