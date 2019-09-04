#pragma once
#include <sstream>
#include <vector>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/LevelDBHandler.h"

class LevelDBDeletePlan : public SingleChildPlan
{
public:
	LevelDBDeletePlan(const TableInfo* pTable, ExecutionPlan* pPlan);

	virtual void explain(std::vector<std::string>& rows)override {
		SingleChildPlan::explain(rows);
		std::ostringstream os;
		os << "leveldb:delete " << m_pTable->getName();
		rows.push_back(os.str());
	}

	virtual std::string getInfoString() override {
		return ConcateToString("DELETE ", m_iDeleteRows);
	}

	virtual int getResultColumns() override	{
		return 0;
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

private:
	const TableInfo* m_pTable;

	size_t m_iDeleteRows = 0;
	int m_iRowkeyProjection = -1;

	ExecutionResult m_rowkey;
	LevelDBBatch m_batch;
};
