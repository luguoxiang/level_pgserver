#pragma once
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "execution/ParseTools.h"
#include "execution/BasePlan.h"
#include "execution/WorkThreadInfo.h"

class ShowTables: public LeafPlan {
public:
	ShowTables() : LeafPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows) override{
		rows.push_back("ShowTables");
	}

	virtual std::string getInfoString() override;

	virtual std::string_view getProjectionName(size_t index) override{
		return index == 0 ? "TableName" : "Info";
	}
	virtual DBDataType getResultType(size_t index)override {
		return DBDataType::STRING;
	}

	virtual void begin()override;

	virtual bool next()override;

	virtual void end() override{
	}

	virtual int getResultColumns()override {
		return 2;
	}

	virtual void getResult(size_t index, ExecutionResult& result)override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		columns.push_back("TableName");
		columns.push_back("Info");
	}

	virtual int addProjection(const ParseNode* pNode)override {
		assert(pNode);
		if (pNode->m_type != NodeType::NAME)
			return -1;
		if (Tools::case_equals(pNode->m_sValue, "TableName") )
			return 0;
		if (Tools::case_equals(pNode->m_sValue, "Info") )
			return 1;
		return -1;
	}
private:
	size_t m_iIndex;
	std::vector<TableInfo*> m_tables;
};
