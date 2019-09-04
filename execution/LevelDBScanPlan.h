#pragma once
#include <vector>
#include "common/ConfigInfo.h"
#include "common/ParseNode.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"
#include "execution/KeySearchRange.h"


class LevelDBScanPlan : public LeafPlan
{
	friend class ScanRange;
public:
	LevelDBScanPlan(const TableInfo* pTable);

	virtual void explain(std::vector<std::string>& rows, size_t depth)override;

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
	const DataRow getStartRow() const {
		assert(m_pSearchRange != nullptr);
		return m_pSearchRange->getStartRow();
	}

	const DataRow getEndRow() const {
		assert(m_pSearchRange != nullptr);
		return m_pSearchRange->getEndRow();
	}
	bool startInclusive() const {
		assert(m_pSearchRange != nullptr);
		return m_pSearchRange->startInclusive();
	}
	bool endInclusive() const {
		assert(m_pSearchRange != nullptr);
		return m_pSearchRange->endInclusive();
	}

	KeySearchRange* getKeySearchRange() {return m_pSearchRange.get();}

	void setLevelDBIterator(LevelDBIteratorPtr& pIter) {
		m_pDBIter = pIter;
	}
private:

	using ValueInfo = std::pair<bool, ExecutionResult>;
	std::vector<ValueInfo> m_columnValues;
	bool m_bProjectValue = false;

	std::vector<ExecutionResult> m_keyValues;

	std::vector<DBDataType> m_keyTypes;
	std::vector<DBDataType> m_valueTypes;
	const TableInfo* m_pTable;
	SortOrder m_order = SortOrder::Any;

	DataRow m_currentKey;
	size_t m_iRows = 0;
	LevelDBIteratorPtr m_pDBIter;
	std::unique_ptr<KeySearchRange> m_pSearchRange;

};
