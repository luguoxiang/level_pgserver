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
private:
	bool addSimplePredicate(const ParseNode* pNode);
	size_t m_iNotEqualIndex;
	std::vector<const ParseNode*> m_equals;
	const ParseNode* m_pStart;
	const ParseNode* m_pEnd;

	const TableInfo* m_pTable;

	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;


};
