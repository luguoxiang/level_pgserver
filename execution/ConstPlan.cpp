#include <sstream>
#include "execution/ConstPlan.h"
#include "DBDataTypeHandler.h"

void ConstPlan::explain(std::vector<std::string>& rows, size_t depth) {
	std::stringstream ss;
	ss<<std::string(depth, '\t');
	ss << "Const " << m_rows.size() << " rows, " << m_columns.size()
			<< " columns";
	rows.push_back(ss.str());
}

DBDataType ConstPlan::getResultType(size_t index) {
	assert(!m_rows.empty());
	NodeType lastType = NodeType::NONE;

	DBDataType result = DBDataType::UNKNOWN;

	for(auto pRow : m_rows) {
		assert(pRow->children() == m_columns.size());
		auto pValue = pRow->getChild(index);

		if(pValue->m_type != lastType && lastType != NodeType::NONE) {
			PARSE_ERROR("value ", pValue->m_sExpr ," at column ", index + 1, " does not match with previous rows:");
		}
		lastType = pValue->m_type;
		result = pValue->getConstResultType();
		assert(result != DBDataType::UNKNOWN);
	}
	return result;

}

void ConstPlan::getResult(size_t index, ExecutionResult& result) {
	assert(m_iCurrent > 0 && m_iCurrent <= m_rows.size());
	auto pRow = m_rows[m_iCurrent - 1];

	assert(pRow->children() == m_columns.size());
	auto pValue = pRow->getChild(index);

	DBDataTypeHandler::getHandler(getResultType(index))->fromNode(pValue, result);
}

int ConstPlan::addProjection(const ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_type != NodeType::NAME)
		return -1;
	for (size_t i = 0; i < m_columns.size(); ++i) {
		if (m_columns[i] == pNode->getString()) {
			return i;
		}
	}
	return -1;
}

void ConstPlan::addRow(const ParseNode* pRow) {
	if (m_rows.empty()) {
		pRow->forEachChild([this](size_t index, auto pChild) {
			m_columns.push_back(absl::StrCat("c", index+1));
		});
	}
	assert(pRow->children() == m_columns.size());
	m_rows.push_back(pRow);
}
