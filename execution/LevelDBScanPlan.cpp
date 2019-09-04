#include <memory>
#include "LevelDBScanPlan.h"
#include "ExecutionException.h"
#include "common/ParseTools.h"
#include "execution/DBDataTypeHandler.h"
#include "execution/LevelDBHandler.h"
#include "execution/DataRow.h"


LevelDBScanPlan::LevelDBScanPlan(const TableInfo* pTable)
	: LeafPlan(PlanType::Scan), m_pTable(pTable)
	, m_keyValues(pTable->getKeyCount())
	, m_currentKey(m_keyTypes)
	, m_keyTypes(pTable->getKeyCount()) {

	for(size_t i = 0;i<pTable->getColumnCount();++i) {
		auto pColumn = m_pTable->getColumn(i);
		if(pColumn->m_iKeyIndex < 0) {
			m_valueTypes.push_back(pColumn->m_type);
		} else {
			m_keyTypes[pColumn->m_iKeyIndex] = pColumn->m_type;
		}
	}

}
void LevelDBScanPlan::setPredicate(const ParseNode* pNode, std::set<std::string_view>& solved) {
	m_pSearchRange = std::make_unique<KeySearchRange>(m_keyTypes,  m_pTable, pNode, &solved);
}

int LevelDBScanPlan::addProjection(const ParseNode* pNode) {
	if(Tools::isRowKeyNode(pNode)) {
		return m_columnValues.size() + m_pTable->getKeyCount();
	} else if(pNode->m_type == NodeType::NAME) {
		auto pColumn = m_pTable->getColumnByName(pNode->getString());
		if(pColumn == nullptr ) {
			PARSE_ERROR("Unknown column: ", pNode->getString());
		}
		if(pColumn->m_iKeyIndex >= 0) {
			return pColumn->m_iKeyIndex;
		}
		assert(pColumn->m_iValueIndex >= 0);
		m_columnValues.emplace_back(pColumn->m_iValueIndex, ExecutionResult{});

		return m_pTable->getKeyCount() + pColumn->m_iValueIndex;
	}
	return -1;
}

void LevelDBScanPlan::begin() {
	m_iRows = 0;

	if(m_pDBIter == nullptr) {
		m_pDBIter = LevelDBHandler::getHandler(m_pTable)->createIterator();
	}

	if(m_pSearchRange == nullptr) {
		m_pSearchRange = std::make_unique<KeySearchRange>(m_keyTypes, m_pTable, nullptr, nullptr);
	}

	if(m_order ==SortOrder::Descend) {
		m_pSearchRange->seekStartReversed(m_pDBIter);
	} else {
		m_pSearchRange->seekStart(m_pDBIter);
	}

}
bool LevelDBScanPlan::next() {
	if(!m_pDBIter->valid()) {
		return false;
	}
	m_currentKey = m_pDBIter->key(m_keyTypes);

	for(size_t i = 0;i<m_keyValues.size();++i) {
		m_currentKey.getResult(i, m_keyValues[i]);
	}

	if(m_order ==SortOrder::Descend) {
		if(m_pSearchRange->exceedEndReversed(m_keyValues)) {
			return false;
		}
	} else if(m_pSearchRange->exceedEnd(m_keyValues)) {
			return false;
	}

	if(!m_columnValues.empty()) {
		auto valueRow = m_pDBIter->value(m_valueTypes);

		for(size_t i=0;i<m_columnValues.size();++i) {
			auto& valueInfo = m_columnValues[i];
			valueRow.getResult(valueInfo.first, valueInfo.second);
		};
	}
	if(m_order ==SortOrder::Descend) {
		m_pDBIter->prev();
	}else {
		m_pDBIter->next();
	}
	++m_iRows;
	return true;
}
void LevelDBScanPlan::end() {
	m_pDBIter = nullptr;
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
	if(m_pSearchRange == nullptr) {
		m_pSearchRange = std::make_unique<KeySearchRange>(m_keyTypes, m_pTable,nullptr, nullptr);
	}

	auto& start = m_pSearchRange->getStartResults();
	auto& end = m_pSearchRange->getEndResults();

	assert(start.size() == end.size());
	std::string s = "leveldb:scan ";
	s.append(m_pTable->getName());
	s.append(", cost:");

	auto startRow = m_pSearchRange->getStartRow();
	auto endRow = m_pSearchRange->getEndRow();
	auto cost = LevelDBHandler::getHandler(m_pTable)->getCost(startRow, endRow);

	s.append(std::to_string(cost));

	if(m_order == SortOrder::Descend) {
		s.append(", reverse");
	}
	rows.push_back(s);
	s = "  range:";
	s.append(m_pSearchRange->startInclusive()?"[":"(");
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
	s.append(m_pSearchRange->endInclusive()?"]":")");
	rows.push_back(s);

}

void LevelDBScanPlan::getResult(size_t columnIndex, ExecutionResult& result) {
	if(columnIndex < m_pTable->getKeyCount()) {
		result = m_keyValues[columnIndex];
	} else if(columnIndex - m_pTable->getKeyCount() < m_columnValues.size()) {
		result = m_columnValues[columnIndex - m_pTable->getKeyCount()].second;
	} else {
		assert(columnIndex == m_pTable->getKeyCount() + m_columnValues.size());
		result.setStringView(std::string_view(m_currentKey.data(), m_currentKey.size()));
	}
}
