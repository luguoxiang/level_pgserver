#pragma once
#include <sstream>
#include <vector>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"
#include "common/ParseNode.h"
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

	bool addPredicate(const ParseNode* pNode, std::vector<const ParseNode*>& unsolved);

	void setDBIterator(LevelDBIteratorPtr& pIter) {
		m_pDBIter = pIter;
	}
private:
	bool addSimplePredicate(const ParseNode* pNode);
	void doAddPredicate(const ParseNode* pNode, std::vector<const ParseNode*>& unsolved);

	struct PredicateInfo {
		const ParseNode* m_pStart = nullptr;
		const ParseNode* m_pEnd = nullptr;
		Operation m_startOp = Operation::NONE;
		Operation m_endOp = Operation::NONE;

		const ParseNode* m_pStartExpr = nullptr;
		const ParseNode* m_pEndExpr = nullptr;
	};
	std::vector<PredicateInfo> m_predicates;

	const TableInfo* m_pTable;

	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;
	LevelDBIteratorPtr m_pDBIter;

};
