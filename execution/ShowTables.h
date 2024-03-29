#pragma once
#include <stdio.h>
#include <vector>
#include <absl/strings/str_cat.h>
#include "common/ParseTools.h"
#include "execution/BasePlan.h"

class ShowTables: public LeafPlan {
public:
	ShowTables() : LeafPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override{
		rows.push_back(absl::StrCat(std::string(depth, '\t'), "ShowTables"));
	}

	virtual std::string getInfoString() override;

	virtual std::string_view getProjectionName(size_t index) override{
		return index == 0 ? "TableName" : "Info";
	}
	virtual DBDataType getResultType(size_t index)override {
		return DBDataType::STRING;
	}

	virtual void begin(const std::atomic_bool& bTerminated)override;

	virtual bool next(const std::atomic_bool& bTerminated)override;

	virtual void end() override{
	}

	virtual int getResultColumns()override {
		return 2;
	}

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type)override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		columns.push_back("TableName");
		columns.push_back("Info");
	}

	virtual int addProjection(const ParseNode* pNode)override;
private:
	size_t m_iIndex = 0;
	std::vector<TableInfo*> m_tables;
};
