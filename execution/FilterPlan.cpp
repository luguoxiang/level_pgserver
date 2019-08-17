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
		std::ostringstream os;
		os << "Unsupported operation " << iOpCode;
		throw new ParseException(os.str());
		return 0;
	}
}
}
bool FilterPlan::next() {
	while (m_pPlan->next()) {
		bool bMatch = true;
		for (size_t i = 0; i < m_predicate.size(); ++i) {
			PredicateInfo& info = m_predicate[i];
			int iSubIndex = info.m_iSubIndex;
			DBDataType type = m_pPlan->getResultType(iSubIndex);
			ResultInfo result;
			m_pPlan->getResult(iSubIndex, &result);

			if (result.isNull()) {
				bMatch = false;
				break;
			}
			if (type == DBDataType::STRING && info.m_iOpCode == LIKE) {
				if (info.m_pValue->m_type != NodeType::STR) {
					std::ostringstream os;
					os << "Wrong data type for "<< info.m_pValue->m_sExpr <<", expect string";
					throw new ParseException(os.str());
				}
				auto pos = result.getString().find(info.m_pValue->m_sValue);
				if (pos == std::string::npos) {
					bMatch = false;
					break;
				}
			} else {
				int n = result.compare(info.m_pValue, type);
				if (!checkFilter(info.m_iOpCode, n)) {
					bMatch = false;
					break;
				}
			}
		}
		if (!bMatch)
			continue;
		++m_iCurrent;
		return true;
	}
	return false;
}

void FilterPlan::addPredicate(ParseNode* pPredicate) {
	if (pPredicate->m_type != NodeType::OP || pPredicate->children() != 2) {
		std::ostringstream os;
		os << "Unsupported predicate " << pPredicate->m_sExpr;
		throw new ParseException(os.str());
	}
	assert(pPredicate);
	assert(pPredicate->children() == 2);
	assert(pPredicate->m_type == NodeType::OP);
	PredicateInfo info;
	info.m_sColumn = pPredicate->m_children[0]->m_sExpr;
	info.m_sExpr = pPredicate->m_sExpr;
	int i = m_pPlan->addProjection(pPredicate->m_children[0]);
	if (i < 0) {
		std::ostringstream os;
		os << "Unrecognized column " << info.m_sColumn;
		throw new ParseException(os.str());
	}
	info.m_iSubIndex = i;
	info.m_iOpCode = OP_CODE(pPredicate);
	info.m_pValue = pPredicate->m_children[1];
	m_predicate.push_back(info);
}
