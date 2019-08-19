#pragma once
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "execution/ParseTools.h"
#include "execution/ExecutionPlan.h"
#include "execution/WorkThreadInfo.h"

class ShowTables: public ExecutionPlan {
public:
	ShowTables() : ExecutionPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows) override{
		rows.push_back("ShowTables");
	}

	virtual std::string getInfoString() override;

	virtual std::string getProjectionName(size_t index) override{
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

	virtual void getResult(size_t index, ExecutionResult* pInfo)override;

	virtual void getAllColumns(std::vector<std::string>& columns)override {
		columns.push_back("TableName");
		columns.push_back("Info");
	}

	virtual int addProjection(ParseNode* pNode)override {
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
