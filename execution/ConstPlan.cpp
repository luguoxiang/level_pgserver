#include "execution/ConstPlan.h"
#include <sstream>

void ConstPlan::explain(std::vector<std::string>& rows) {
	std::stringstream ss;
	ss << "Const " << m_rows.size() << " rows, " << m_columns.size()
			<< " columns";
	rows.push_back(ss.str());
}

DBDataType ConstPlan::getResultType(size_t index) {
	assert(!m_rows.empty());
	ParseNode* pRow = m_rows[0];
	assert(pRow->children() == m_columns.size());
	switch (pRow->m_children[index]->m_type) {
	case NodeType::INT:
		return DBDataType::INT64;
	case NodeType::STR:
		return DBDataType::STRING;
	case NodeType::FLOAT:
		return DBDataType::DOUBLE;
	case NodeType::DATE:
		return pRow->m_children[index]->m_sValue.length() < 12 ?
				DBDataType::DATE : DBDataType::DATETIME;
	default:
		assert(0);
		return DBDataType::UNKNOWN;
	}
}

void ConstPlan::getResult(size_t index, ExecutionResult* pInfo) {
	assert(m_iCurrent > 0 && m_iCurrent <= m_rows.size());
	ParseNode* pRow = m_rows[m_iCurrent - 1];
	assert(pRow->children() == m_columns.size());
	ParseNode* pValue = pRow->m_children[index];
	switch (pValue->m_type) {
	case NodeType::INT:
		pInfo->setInt(pValue->m_iValue);
		break;
	case NodeType::DATE: {
		struct timeval time;
		time.tv_sec = (pValue->m_iValue / 1000000);
		time.tv_usec = (pValue->m_iValue % 1000000);
		pInfo->setTime(time);
		break;
	}
	case NodeType::FLOAT: {
		float fValue = strtof(pValue->m_sValue.c_str(), nullptr);
		pInfo->setDouble(fValue);
		break;
	}
	case NodeType::STR:
		pInfo->setStringView(pValue->m_sValue);
		break;
	default:
		throw new ParseException("wrong const value type %d");
	}
}

int ConstPlan::addProjection(ParseNode* pNode) {
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

void ConstPlan::addRow(ParseNode* pRow) {
	if (m_rows.empty()) {
		for (size_t i = 0; i < pRow->children(); ++i) {
			std::stringstream ss;
			ss << i + 1;
			m_columns.push_back(ss.str());
		}
	}
	assert(pRow->children() == m_columns.size());
	m_rows.push_back(pRow);
}
