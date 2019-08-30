#pragma once
#include <sstream>
#include <vector>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"
#include "common/ParseNode.h"

struct ScanRange {
	ScanRange(const TableInfo* pTable, const ParseNode* pNode, ExecutionBuffer* pBuffer);
	struct PredicateInfo {
		const DBColumnInfo* m_pKeyColumn;
		const ParseNode* m_pValue;
		Operation m_op = Operation::NONE;
		std::string_view m_sExpr;

		//non-equal end range is set to m_predicates[index +1]
		bool isPrevEndRange() {
			switch(m_op){
			case Operation::COMP_LT:
			case Operation::COMP_LE:
				return true;
			default:
				return false;
			}
		}
	};
	std::vector<PredicateInfo> m_predicates;

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
	std::string_view m_sStartRow;
	std::string_view m_sEndRow;
private:
	void visit(const ParseNode* pPredicate);
	void setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate);
	const TableInfo* m_pTable;
};

class LevelDBScanPlan : public LeafPlan
{
public:
	LevelDBScanPlan(const TableInfo* pTable);

	virtual void explain(std::vector<std::string>& rows)override {
		std::ostringstream os;
		os << "leveldb:scan " << m_pTable->getName();
		rows.push_back(os.str());
	}

	virtual std::string getInfoString() override {
		return ConcateToString("SELECT ", m_iRows);
	}

	virtual int getResultColumns() override	{
		return 0;
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int addProjection(const ParseNode* pColumn) override;

	bool addPredicate(const ParseNode* pNode);

private:
	using ScanRangePtr = std::shared_ptr<ScanRange>;
	std::vector<ScanRangePtr> m_scanRanges;
	const TableInfo* m_pTable;

	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;
	LevelDBIteratorPtr m_pDBIter;

};
