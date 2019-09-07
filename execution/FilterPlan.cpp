#include "execution/FilterPlan.h"
#include "execution/DBDataTypeHandler.h"
#include "common/ParseException.h"


bool FilterPlan::evaluate(const PredicateInfo& info) {
	ExecutionResult result1, result2;
	DBDataType type = DBDataType::UNKNOWN;
	if(info.m_iRightIndex >= 0) {
		type = m_pPlan->getResultType(info.m_iRightIndex);
		m_pPlan->getResult(info.m_iRightIndex, result2);
	}
	if (info.m_iLeftIndex >= 0) {
		type = m_pPlan->getResultType(info.m_iLeftIndex);
		m_pPlan->getResult(info.m_iLeftIndex, result1);
	}
	assert(type != DBDataType::UNKNOWN);
	if(info.m_iLeftIndex < 0) {
		result1 = info.m_leftConst;
	}
	if(info.m_iRightIndex < 0) {
		result2 = info.m_rightConst;
	}

	assert(!result1.isNull());
	assert(!result2.isNull());

	if (info.m_op == Operation::LIKE) {
		auto pos = result1.getString().find(result2.getString());
		return pos != std::string::npos;
	} else {
		int n = result1.compare(result2, type);
		switch (info.m_op) {
		case Operation::COMP_EQ:
			return n == 0;
		case Operation::COMP_NE:
			return n != 0;
		case Operation::COMP_LE:
			return n <= 0;
		case Operation::COMP_LT:
			return n < 0;
		case Operation::COMP_GT:
			return n > 0;
		case Operation::COMP_GE:
			return n >= 0;
		default:
			PARSE_ERROR("Unsupported operation ", info.m_sExpr);
			return 0;
		}
	}
}
bool FilterPlan::next() {
	while (m_pPlan->next()) {
		for (auto& pAnd : m_predicatesInOr) {
			bool bMatch = true;
			for (auto& info : *pAnd) {
				if (!evaluate(info)) {
					bMatch = false;
					break;
				}
			}
			if (bMatch) {
				//one of predicate in OR list match
				++m_iCurrent;
				return true;
			}
		}
	}
	return false;
}

void FilterPlan::doAddPredicate(std::vector<PredicateInfo>& andList, const ParseNode* pPredicate, std::set<std::string_view>* pIgnore) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}

	auto op = pPredicate->getOp();
	if (op == Operation::AND) {
		pPredicate->forEachChild([this,&andList, pIgnore](size_t index, auto pChild){
			doAddPredicate(andList, pChild, pIgnore);
		});
		return;
	}
	assert(op != Operation::OR);
	if (pPredicate->children() != 2) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}

	if(pIgnore != nullptr && pIgnore->find(pPredicate->m_sExpr) != pIgnore->end()) {
		return;
	}
	PredicateInfo info = {};
	info.m_sExpr = pPredicate->m_sExpr;
	info.m_op = op;
	auto pLeft = pPredicate->getChild(0);
	auto pRight = pPredicate->getChild(1);
	info.m_iLeftIndex = m_pPlan->addProjection(pLeft);
	info.m_iRightIndex =  m_pPlan->addProjection(pRight);

	if (info.m_iLeftIndex < 0 && info.m_iRightIndex < 0) {
		PARSE_ERROR("Unrecognized predicate ", info.m_sExpr);
	}
	if(info.m_iLeftIndex < 0) {
		if(pLeft->isConst()) {
			auto type = m_pPlan->getResultType(info.m_iRightIndex);
			auto pHandler = DBDataTypeHandler::getHandler(type);
			assert(pHandler!= nullptr);
			pHandler->fromNode(pLeft, info.m_leftConst);
		}else{
			PARSE_ERROR("Unsupported predicate ", info.m_sExpr);
		}
	}
	if(info.m_iRightIndex < 0) {
		if(pRight->isConst()) {
			auto type = m_pPlan->getResultType(info.m_iLeftIndex);
			auto pHandler = DBDataTypeHandler::getHandler(type);
			assert(pHandler!= nullptr);
			pHandler->fromNode(pRight, info.m_rightConst);
		}else{
			PARSE_ERROR("Unsupported predicate ", info.m_sExpr);
		}
	}

	andList.push_back(info);
}

bool FilterPlan::addPredicate(const ParseNode* pPredicate, std::set<std::string_view>* pIgnore) {
	m_predicatesInOr.emplace_back(new std::vector<PredicateInfo>());
	auto& pAnd = m_predicatesInOr.back();
	doAddPredicate(*pAnd, pPredicate, pIgnore);
	return !pAnd->empty();
}

void FilterPlan::setPredicate(const ParseNode* pNode) {
	m_predicatesInOr.clear();
	if(pNode == nullptr) {
		return;
	}
	if (pNode->getOp() == Operation::OR) {
		pNode->forEachChild([this](size_t index, auto pChild) {
			//should be rewritten
			assert(pChild->getOp() != Operation::OR);
			addPredicate(pChild);
		});

	} else {
		addPredicate(pNode);
	}
}
