#pragma once
#include <sstream>
#include <vector>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"

class LevelDBInsertPlan : public SingleChildPlan
{
public:
	LevelDBInsertPlan(const TableInfo* pTable, ExecutionPlanPtr& pPlan);

	virtual void explain(std::vector<std::string>& rows)override {
		SingleChildPlan::explain(rows);
		std::ostringstream os;
		os << "leveldb:insert " << m_pTable->getName();
		rows.push_back(os.str());
	}

	virtual std::string getInfoString() override {
		return ConcateToString("INSERT 0 ", m_iInsertRows);
	}

	virtual int getResultColumns() override	{
		return 0;
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

private:
	const TableInfo* m_pTable;

	size_t m_iInsertRows = 0;

	std::unique_ptr<ExecutionBuffer> m_pBuffer;
	LevelDBBatch m_batch;
};
