#pragma once

#include "common/BuildPlan.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include <sstream>
#include <fstream>

class ReadFilePlan: public ExecutionPlan {
public:
	ReadFilePlan(const std::string& sPath, const std::string& seperator) :
			ExecutionPlan(PlanType::ReadFile), m_sPath(sPath), m_seperator(seperator) {
	}


	virtual void explain(std::vector<std::string>& rows) override;

	virtual void begin() override;

	virtual bool next() override;

	virtual void end() override;

	virtual void cancel() override{
		m_bCancel = true;
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_columns.size();
	}

	virtual std::string getProjectionName(size_t index) override{
		return m_columns[index]->m_sName;
	}

	virtual DBDataType getResultType(size_t index)override {
		return m_columns[index]->m_type;
	}

	virtual std::string getInfoString() override{
		std::ostringstream os;
		os << "SELECT " << m_iRowCount;
		return os.str();
	}

	virtual void getResult(size_t index, ResultInfo* pInfo)override {
		*pInfo = m_result[index];
	}

	virtual void getAllColumns(std::vector<std::string>& columns)override {
		for (auto pColumn: m_columns) {
			columns.push_back(pColumn->m_sName);
		}
	}

	virtual int addProjection(ParseNode* pNode) override;

	void addColumn(DBColumnInfo* pColumn) {
		m_columns.push_back(pColumn);
		m_result.push_back(ResultInfo());
	}

private:
	std::vector<DBColumnInfo*> m_columns;
	std::vector<ResultInfo> m_result;
	int64_t m_iRowCount = 0;
	std::string m_sPath;

	std::unique_ptr<std::ifstream> m_pFile;
	bool m_bCancel = false;
	std::string m_seperator;
};
