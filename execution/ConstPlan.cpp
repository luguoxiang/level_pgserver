#include <sstream>
#include "execution/ConstPlan.h"
#include "DBDataTypeHandler.h"

void ConstPlan::explain(std::vector<std::string>& rows) {
	std::stringstream ss;
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
		switch (lastType) {
		case NodeType::BOOL:
			result = DBDataType::INT8;
			break;
		case NodeType::INT:
			result = DBDataType::INT64;
			break;
		case NodeType::STR:
			result = DBDataType::STRING;
			break;
		case NodeType::BINARY:
			result = DBDataType::BYTES;
			break;
		case NodeType::FLOAT:
			result = DBDataType::DOUBLE;
			break;
		case NodeType::DATE:
			result = DBDataType::DATETIME;
			break;
		default:
			assert(0);
			PARSE_ERROR("Wrong value: ", pValue->m_sExpr);
		}
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
		if (m_columns[i] == pNode->m_sValue) {
			return i;
		}
	}
	return -1;
}

void ConstPlan::addRow(const ParseNode* pRow) {
	if (m_rows.empty()) {
		for (size_t i = 0; i < pRow->children(); ++i) {
			m_columns.push_back(std::to_string(i+1));
		}
	}
	assert(pRow->children() == m_columns.size());
	m_rows.push_back(pRow);
}
