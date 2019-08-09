#pragma once

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "common/ConfigInfo.h"
#include "execution/ExecutionPlan.h"
#include "execution/WorkThreadInfo.h"

class ShowColumns: public ExecutionPlan {
public:
	ShowColumns(TableInfo* pEntry) :
			ExecutionPlan(Other), m_pEntry(pEntry) {
	}

	virtual void explain(std::vector<std::string>& rows) {
		rows.push_back("ShowColumns");
	}

	virtual void begin();

	virtual bool next();

	virtual void end() {
	}

	virtual void getResult(size_t index, ResultInfo* pInfo);

	virtual const char* getProjectionName(size_t index);

	virtual DBDataType getResultType(size_t index);

	virtual int getResultColumns();

	virtual void getInfoString(char* szBuf, int len);

	virtual void getAllColumns(std::vector<const char*>& columns);

	virtual int addProjection(ParseNode* pNode);
private:
	size_t m_iIndex;
	TableInfo* m_pEntry;
};
