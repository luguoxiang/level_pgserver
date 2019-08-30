#pragma once
#include <sstream>
#include <vector>
#include <list>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"
#include "common/ParseNode.h"

struct KeyPredicateInfo {
	const DBColumnInfo* m_pKeyColumn;
	const ParseNode* m_pValue;
	Operation m_op = Operation::NONE;
	std::string_view m_sExpr;
	bool m_bResult = false;

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

class LevelDBScanPlan : public LeafPlan
{
	friend class ScanRange;
public:
	LevelDBScanPlan(const TableInfo* pTable);

	virtual void explain(std::vector<std::string>& rows)override {
		std::string s = "leveldb:scan  ";
		s.append(m_pTable->getName());
		for(auto& info:m_predicates) {
			if(info.m_op != Operation::NONE) {
				s.append(info.m_sExpr);
				s.append(" and ");
			}
		}
		s.erase (s.end()- 5, s.end());
		rows.push_back(s);
	}
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

	void setPredicate(const ParseNode* pNode, std::vector<const ParseNode*>& unsolved);

private:
	std::vector<KeyPredicateInfo> m_predicates;

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

	void doSetPredicate(const ParseNode* pPredicate, std::vector<const ParseNode*>& unsolved);
	bool setPredicateInfo(Operation op, const ParseNode* pKey, const ParseNode* pValue, const ParseNode* pPredicate);

	std::vector<bool> m_projection;
	std::vector<ExecutionResult> m_columnValues;

	std::vector<DBDataType> m_keyTypes;
	const TableInfo* m_pTable;

	DataRow m_currentRow;
	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;
	LevelDBIteratorPtr m_pDBIter;

};
