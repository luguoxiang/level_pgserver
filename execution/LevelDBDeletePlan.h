#pragma once
#include <vector>
#include <absl/strings/substitute.h>
#include <absl/strings/str_cat.h>

#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/LevelDBHandler.h"

class LevelDBDeletePlan : public SingleChildPlan
{
public:
	LevelDBDeletePlan(const TableInfo* pTable, ExecutionPlan* pPlan);

	virtual void explain(std::vector<std::string>& rows, size_t depth)override {
		rows.push_back(absl::Substitute("$0leveldb:delete $1", std::string(depth, '\t'), m_pTable->getName()));
		SingleChildPlan::explain(rows, depth);
	}

	virtual std::string getInfoString() override {
		return absl::StrCat("DELETE ", m_iDeleteRows);
	}

	virtual int getResultColumns() override	{
		return 0;
	}

	virtual void begin(const std::atomic_bool& bTerminated) override;
	virtual bool next(const std::atomic_bool& bTerminated) override;
	virtual void end() override;

private:
	const TableInfo* m_pTable;

	size_t m_iDeleteRows = 0;
	int m_iRowkeyProjection = -1;

	ExecutionResult m_rowkey;
	LevelDBBatch m_batch;
};
