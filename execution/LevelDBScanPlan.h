#pragma once
#include <sstream>
#include <vector>
#include <set>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"
#include "common/ParseNode.h"

struct KeyPredicateInfo {
	int m_iKeyIndex = -1;
	const ParseNode* m_pValue = nullptr;;
	Operation m_op = Operation::NONE;
	std::string_view m_sExpr;
	bool m_bResult = false;
};

class LevelDBScanPlan : public LeafPlan
{
	friend class ScanRange;
public:
	LevelDBScanPlan(const TableInfo* pTable);

	virtual void explain(std::vector<std::string>& rows)override;

	virtual std::string getInfoString() override {
		return ConcateToString("SELECT ", m_iRows);
	}

	virtual int getResultColumns() override	{
		return m_columnValues.size()  + 1;
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int addProjection(const ParseNode* pColumn) override;

	void setPredicate(const ParseNode* pNode, std::set<std::string_view>& solved);

	uint64_t getCost();

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order)override;

	virtual void getResult(size_t columnIndex, ExecutionResult& result)override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		for (size_t i=0;i<m_pTable->getColumnCount();++i) {
			columns.push_back(m_pTable->getColumn(i)->m_name);
		}
	}
	virtual DBDataType getResultType(size_t index)override{
		if(index == m_pTable->getColumnCount()) {
			return DBDataType::BYTES;
		} else  {
			assert(index < m_pTable->getColumnCount());
			return m_pTable->getColumn(index)->m_type;
		}
	}
	const DataRow& getStartRow() const {
		return m_startRow;
	}

	const DataRow& getEndRow() const {
		return m_endRow;
	}
	bool startInclusive() const {
		return m_bStartInclusive;
	}
	bool endInclusive() const {
		return m_bEndInclusive;
	}
private:
	std::vector<KeyPredicateInfo> m_predicates;
	KeyPredicateInfo m_endPredicate;
	bool isSeekToFirst() {
		switch(m_predicates[0].m_op){
		case Operation::NONE:
		case Operation::COMP_GT:
		case Operation::COMP_GE:
			return true;
		default:
			return false;
		}
	}
	bool isSeekToLast() {
		switch(m_predicates[0].m_op){
		case Operation::NONE:
		case Operation::COMP_LT:
		case Operation::COMP_LE:
			return true;
		default:
			return false;
		}
	}
	bool isFullScan() {
		return m_predicates[0].m_op == Operation::NONE;
	}
	DataRow m_startRow;
	DataRow m_endRow;
	bool m_bStartInclusive = true;
	bool m_bEndInclusive = true;

	void doSetPredicate(const ParseNode* pPredicate);
	void setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate);

	std::vector<bool> m_projection;
	std::vector<ExecutionResult> m_columnValues;

	std::vector<DBDataType> m_keyTypes;
	std::vector<DBDataType> m_valueTypes;
	const TableInfo* m_pTable;
	SortOrder m_order = SortOrder::Any;

	DataRow m_currentRow;
	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;
	LevelDBIteratorPtr m_pDBIter;

};
