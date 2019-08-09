#pragma once

#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include <iostream>

class ReadFilePlan: public ExecutionPlan {
public:
	ReadFilePlan(const char* pszPath, const char* seperator) :
			ExecutionPlan(ReadFile), m_pszPath(pszPath), m_pHandle(nullptr), m_iRowCount(
					0), m_bCancel(false) {
		m_seperator[0] = seperator[0];
		m_seperator[1] = 0;
	}

	ReadFilePlan(const char* pszPath, int seperator) :
			ExecutionPlan(ReadFile), m_pszPath(pszPath), m_pHandle(nullptr), m_iRowCount(
					0), m_bCancel(false) {
		m_seperator[0] = seperator;
		m_seperator[1] = 0;
	}
	virtual ~ReadFilePlan();

	virtual void explain(std::vector<std::string>& rows);

	virtual void begin();

	virtual bool next();

	virtual void end();

	virtual void cancel() {
		m_bCancel = true;
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns() {
		return m_columns.size();
	}

	virtual const char* getProjectionName(size_t index) {
		return m_columns[index]->getName();
	}

	virtual DBDataType getResultType(size_t index) {
		return m_columns[index]->m_type;
	}

	virtual void getInfoString(char* szBuf, int len) {
		snprintf(szBuf, len, "SELECT %llu", m_iRowCount);
	}

	virtual void getResult(size_t index, ResultInfo* pInfo) {
		*pInfo = m_result[index];
	}

	virtual void getAllColumns(std::vector<const char*>& columns) {
		for (size_t i = 0; i < m_columns.size(); ++i) {
			columns.push_back(m_columns[i]->getName());
		}
	}

	virtual int addProjection(ParseNode* pNode);

	void addColumn(DBColumnInfo* pColumn) {
		m_columns.push_back(pColumn);
		m_result.push_back(ResultInfo());
	}

private:
	std::vector<DBColumnInfo*> m_columns;
	std::vector<ResultInfo> m_result;
	int64_t m_iRowCount;
	const char* m_pszPath;
	FILE* m_pHandle;
	char m_szBuf[4096];
	bool m_bCancel;
	char m_seperator[2];
};
