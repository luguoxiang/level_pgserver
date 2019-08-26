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
	auto pRow = m_rows[0];
	assert(pRow->children() == m_columns.size());
	switch (pRow->getChild(index)->m_type) {
	case NodeType::INT:
		return DBDataType::INT64;
	case NodeType::STR:
		return DBDataType::STRING;
	case NodeType::BINARY:
		return DBDataType::BYTES;
	case NodeType::FLOAT:
		return DBDataType::DOUBLE;
	case NodeType::DATE:
		return pRow->getChild(index)->m_sValue.length() < 12 ?
				DBDataType::DATE : DBDataType::DATETIME;
	default:
		assert(0);
		return DBDataType::UNKNOWN;
	}
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
