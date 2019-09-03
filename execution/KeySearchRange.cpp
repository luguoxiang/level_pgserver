#include "KeySearchRange.h"
#include "execution/DBDataTypeHandler.h"
#include <glog/logging.h>

KeySearchRange::KeySearchRange(std::set<std::string_view>& solved,
		std::vector<DBDataType>& keyTypes,
		const TableInfo* pTable,
		const ParseNode* pNode)
	: m_solved(solved)
	, m_pTable(pTable)
	, m_keyTypes(keyTypes)
	, m_startKeyResults(keyTypes.size())
	, m_endKeyResults(keyTypes.size())
	, m_predicates(keyTypes.size()) {

	doSetPredicate(pNode);

	bool keyValid = true;

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
					m_solved.insert(m_endPredicate.m_sExpr);
				}
			}
			break;
		default://NONE
			break;
		}
		switch(info.m_op) {
		case Operation::COMP_EQ:
			m_solved.insert(info.m_sExpr);
			break;
		case Operation::COMP_GT:
		case Operation::COMP_GE:
			m_bStartInclusive = (info.m_op == Operation::COMP_GE);
			keyValid = false;
			m_solved.insert(info.m_sExpr);
			break;
		case Operation::NONE:
			keyValid = false;
			break;
		default:
			assert(0);
		}
	}


	bool nextStartMax = false;
	bool nextEndMax = false;
	for(size_t i=0;i<keyTypes.size();++i) {
		auto pColumn = pTable->getKeyColumn(i);

		auto pHandler = DBDataTypeHandler::getHandler(keyTypes[i]);
		assert(pHandler);
		auto& info = m_predicates[i];

		switch(info.m_op) {
		case Operation::COMP_EQ:
			pHandler->fromNode(info.m_pValue, m_startKeyResults[i]);
			m_endKeyResults[i] = m_startKeyResults[i];
			DLOG(INFO) << "Key search " << info.m_sExpr << ", KeyIndex" << info.m_iKeyIndex;
			continue;
		case Operation::COMP_GE:
			pHandler->fromNode(info.m_pValue, m_startKeyResults[i]);
			//key columns has larger index should be set to
			//minimum value to include all possible keys have same ith key column value
			nextStartMax = false;
			DLOG(INFO) << "Key search " << info.m_sExpr<< ", KeyIndex=" << info.m_iKeyIndex;
			break;
		case Operation::COMP_GT:
			pHandler->fromNode(info.m_pValue, m_startKeyResults[i]);
			//key columns has larger index should be set to
			//maximum value to exclude all possible keys have same ith key column value
			nextStartMax = true;
			DLOG(INFO) << "Key search " << info.m_sExpr<< ", KeyIndex=" << info.m_iKeyIndex;
			break;
		default:
			if(nextStartMax) {
				pHandler->setToMax(m_startKeyResults[i]);
			} else {
				pHandler->setToMin(m_startKeyResults[i]);
			}
			break;
		}
		if(i == m_endPredicate.m_iKeyIndex) {
			DLOG(INFO) << "Key search " << m_endPredicate.m_sExpr<< ", KeyIndex=" << m_endPredicate.m_iKeyIndex;
			pHandler->fromNode(m_endPredicate.m_pValue, m_endKeyResults[i]);
			nextEndMax = (m_endPredicate.m_op == Operation::COMP_LE);
		} else {
			if(nextEndMax) {
				pHandler->setToMax(m_endKeyResults[i]);
			} else{
				pHandler->setToMin(m_endKeyResults[i]);
			}
		}
	}

	DataRow row(keyTypes);

	size_t rowSize = row.computeSize(m_startKeyResults);
	m_sStartRow.reserve(rowSize);
	row.copy(m_startKeyResults, (std::byte*)m_sStartRow.data());

	rowSize = row.computeSize(m_endKeyResults);
	m_sEndRow.reserve(rowSize);

	row.copy(m_endKeyResults,(std::byte*) m_sEndRow.data());
}

void KeySearchRange::doSetPredicate(const ParseNode* pPredicate) {
	if (pPredicate == nullptr || pPredicate->m_type != NodeType::OP) {
		return;
	}

	auto op = pPredicate->getOp();

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

	if(pLeft->m_type == NodeType::NAME && pRight->isConst()) {
		setPredicateInfo(op, pLeft, pRight, pPredicate);

	} else if (pRight->m_type == NodeType::NAME && pLeft->isConst()) {
		setPredicateInfo(reverseOp, pRight, pLeft, pPredicate);
	}
}

void KeySearchRange::setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate) {
	auto pKeyColumn = m_pTable->getColumnByName(pKey->m_sValue);
	if(pKeyColumn->m_iKeyIndex < 0) {
		return;
	}

	assert(pKeyColumn->m_iKeyIndex < m_predicates.size());

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
