#pragma once
#include <vector>
#include <absl/strings/substitute.h>
#include <absl/strings/str_cat.h>

#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/LevelDBHandler.h"

class LevelDBInsertPlan : public SingleChildPlan
{
public:
	LevelDBInsertPlan(const TableInfo* pTable, ExecutionPlan* pPlan);

	virtual void explain(std::vector<std::string>& rows, size_t depth)override {
		rows.push_back(absl::Substitute("$0leveldb:insert $1", std::string(depth, '\t'), m_pTable->getName() ));

		SingleChildPlan::explain(rows, depth);
	}

	virtual std::string getInfoString() override {
		return absl::StrCat("INSERT 0 ", m_iInsertRows);
	}

	virtual int getResultColumns() override	{
		return 0;
	}

	virtual void begin(const std::atomic_bool& bTerminated) override;
	virtual bool next(const std::atomic_bool& bTerminated) override;
	virtual void end() override;

private:
	const TableInfo* m_pTable;

	size_t m_iInsertRows = 0;

	std::vector<std::byte> m_keyBuffer;
	std::vector<std::byte> m_valueBuffer;

	std::vector<DBDataType> m_keyTypes;
	std::vector<DBDataType> m_valueTypes;

	std::vector<ExecutionResult> m_keyResults;
	std::vector<ExecutionResult> m_valueResults;

	LevelDBBatch m_batch;
};
