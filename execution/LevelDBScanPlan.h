#pragma once
#include <sstream>
#include <vector>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/ExecutionBuffer.h"
#include "execution/LevelDBHandler.h"

class LevelDBScanPlan : public SingleChildPlan
{
public:
	LevelDBScanPlan(const TableInfo* pTable, ExecutionPlan* pPlan);

	virtual void explain(std::vector<std::string>& rows)override {
		SingleChildPlan::explain(rows);
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

private:
	struct ScanRange {
		DataRow begin;
		DataRow end;
		size_t keyCount;
		bool includeBegin;
		bool includeEnd;
	};
	const TableInfo* m_pTable;

	size_t m_iRows = 0;
	std::unique_ptr<ExecutionBuffer> m_pBuffer;


};
