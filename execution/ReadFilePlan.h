#pragma once

#include "common/ParseException.h"
#include "execution/BasePlan.h"
#include "execution/ParseTools.h"
#include <sstream>
#include <fstream>
#include <atomic>

class ReadFilePlan: public LeafPlan {
public:
	ReadFilePlan(std::string_view sPath, std::string_view separator,	bool ignoreFirstLine=false) :
		LeafPlan(PlanType::ReadFile),
			m_sPath(sPath.data(), sPath.length()),
			m_ignoreFirstLine(ignoreFirstLine){
		switch(separator.size()) {
		case 0:
			m_separator = ',';
			break;
		case 1:
			m_separator = separator[0];
			if (m_separator == '"') {
				PARSE_ERROR("Should not use \" as file separator");
			}
			break;
		case 2:
			PARSE_ERROR("File separator should be single character");
		}
	}


	virtual void explain(std::vector<std::string>& rows) override;

	virtual void begin() override;

	virtual bool next() override;

	virtual void end() override;

	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_columns.size();
	}

	virtual std::string_view getProjectionName(size_t index) override{
		return m_columns[index]->m_name;
	}

	virtual DBDataType getResultType(size_t index)override {
		return m_columns[index]->m_type;
	}

	virtual std::string getInfoString() override{
		return ConcateToString("SELECT ", m_iRowCount);
	}

	virtual void getResult(size_t index, ExecutionResult& result)override {
		result = m_result[index];
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		for (auto pColumn: m_columns) {
			columns.push_back(pColumn->m_name);
		}
	}

	virtual int addProjection(const ParseNode* pNode) override;

	void addColumn(const DBColumnInfo* pColumn) {
		m_columns.push_back(pColumn);
		m_result.push_back(ExecutionResult());
	}

private:
	void setToken(size_t index, std::string_view token);
	std::vector<const DBColumnInfo*> m_columns;
	std::vector<ExecutionResult> m_result;
	int64_t m_iRowCount = 0;
	std::string m_sPath;
	std::string m_line;

	std::unique_ptr<std::ifstream> m_pFile;
	char m_separator;
	bool m_ignoreFirstLine;

};
