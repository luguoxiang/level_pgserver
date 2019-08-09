#pragma once
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "execution/ExecutionPlan.h"
#include "execution/WorkThreadInfo.h"

class ShowTables: public ExecutionPlan {
public:
	ShowTables() :
			ExecutionPlan(Other) {
	}

	virtual void explain(std::vector<std::string>& rows) {
		rows.push_back("ShowTables");
	}

	virtual void getInfoString(char* szBuf, int len);

	virtual const char* getProjectionName(size_t index) {
		return index == 0 ? "TableName" : "Info";
	}
	virtual DBDataType getResultType(size_t index) {
		return TYPE_STRING;
	}

	virtual void begin();

	virtual bool next();

	virtual void end() {
	}

	virtual int getResultColumns() {
		return 2;
	}

	virtual void getResult(size_t index, ResultInfo* pInfo);

	virtual void getAllColumns(std::vector<const char*>& columns) {
		columns.push_back("TableName");
		columns.push_back("Info");
	}

	virtual int addProjection(ParseNode* pNode) {
		assert(pNode);
		if (pNode->m_iType != NAME_NODE)
			return -1;
		if (strcasecmp(pNode->m_pszValue, "TableName") == 0)
			return 0;
		if (strcasecmp(pNode->m_pszValue, "Info") == 0)
			return 1;
		return -1;
	}
private:
	size_t m_iIndex;
	std::vector<TableInfo*> m_tables;
};
