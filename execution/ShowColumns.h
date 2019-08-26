#pragma once

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "common/ConfigInfo.h"
#include "execution/BasePlan.h"
#include "execution/WorkThreadInfo.h"

class ShowColumns: public LeafPlan {
public:
	ShowColumns(const TableInfo* pEntry) :
		LeafPlan(PlanType::Other), m_pEntry(pEntry) {
	}

	virtual void explain(std::vector<std::string>& rows) override {
		rows.push_back("ShowColumns");
	}

	virtual void begin() override;

	virtual bool next() override;

	virtual void end() override{
	}

	virtual void getResult(size_t index, ExecutionResult& result)override;

	virtual std::string_view getProjectionName(size_t index)override;

	virtual DBDataType getResultType(size_t index)override;

	virtual int getResultColumns() override;

	virtual std::string getInfoString()override;

	virtual void getAllColumns(std::vector<std::string_view>& columns)override;

	virtual int addProjection(const ParseNode* pNode)override;
private:
	size_t m_iIndex;
	const TableInfo* m_pEntry;
};
