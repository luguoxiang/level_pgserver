#include "execution/FilterPlan.h"
#include "common/ParseException.h"

namespace {

bool checkFilter(int iOpCode, int n) {
	switch (iOpCode) {
	case COMP_EQ:
		return n == 0;
	case COMP_NE:
		return n != 0;
	case COMP_LE:
		return n <= 0;
	case COMP_LT:
		return n < 0;
	case COMP_GT:
		return n > 0;
	case COMP_GE:
		return n >= 0;
	default:
		PARSE_ERROR("Unsupported operation ", iOpCode)
		;
		return 0;
	}
}
}
bool FilterPlan::next() {
	while (m_pPlan->next()) {
		for (auto& pAnd : m_predicatesInOr) {
			bool bMatch = true;
			for (auto& info : *pAnd) {
				int iSubIndex = info.m_iSubIndex;
				DBDataType type = m_pPlan->getResultType(iSubIndex);
				ExecutionResult result;
				m_pPlan->getResult(iSubIndex, result);

				if (type == DBDataType::STRING && info.m_iOpCode == LIKE) {
					if (info.m_pValue->m_type != NodeType::STR) {
						PARSE_ERROR("Wrong data type for ",
								info.m_pValue->m_sExpr, ", expect string");
					}
					if (auto pos = result.getString().find(
							info.m_pValue->m_sValue); pos
							== std::string::npos) {
						//one of predicate in AND list mismatch
						bMatch = false;
						break;
					}
				} else {
					int n = result.compare(info.m_pValue, type);
					if (!checkFilter(info.m_iOpCode, n)) {
						//one of predicate in AND list mismatch
						bMatch = false;
						break;
					}
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


void FilterPlan::addPredicate(const std::vector<const ParseNode*>& predicates) {
	m_predicatesInOr.emplace_back(new std::vector<PredicateInfo>());
	auto pAnd = m_predicatesInOr.back().get();
	pAnd->reserve(predicates.size());
	for (auto pPredicate : predicates) {
		assert(pPredicate);
		if (pPredicate->m_type != NodeType::OP
				|| pPredicate->children() != 2) {
			PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
		}

		const ParseNode* pLeft = pPredicate->getChild(0);
		const ParseNode* pRight = pPredicate->getChild(1);

		PredicateInfo info;
		info.m_sExpr = pPredicate->m_sExpr;
		info.m_iOpCode = OP_CODE(pPredicate);

		if(info.m_iOpCode == ANDOP || info.m_iOpCode == OR) {
			PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
		}

		if (int i = m_pPlan->addProjection(pLeft); i >= 0) {
			info.m_sColumn = pLeft->m_sExpr;
			info.m_iSubIndex = i;
			info.m_pValue = pRight;

		} else if (int i = m_pPlan->addProjection(pRight); i >= 0) {
			info.m_sColumn = pRight->m_sExpr;
			info.m_iSubIndex = i;
			info.m_pValue = pLeft;
		} else {
			PARSE_ERROR("Unrecognized column ", info.m_sColumn);
		}
		pAnd->push_back(info);
	}
}
