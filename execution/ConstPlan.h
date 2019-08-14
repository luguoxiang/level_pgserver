#pragma once

#include <sstream>
#include "common/BuildPlan.h"
#include "common/Log.h"
#include "common/ParseException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"

class ConstPlan: public ExecutionPlan {
public:
	ConstPlan() : ExecutionPlan(PlanType::Const), m_iCurrent(0) {
	}

	virtual void explain(std::vector<std::string>& rows)override;

	virtual void begin()override {
		assert(!m_rows.empty());
		m_iCurrent = 0;
	}

	virtual bool next()override {
		return m_iCurrent++ < m_rows.size();
	}

	virtual void end()override {
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns() override {
		return m_columns.size();
	}

	virtual std::string getProjectionName(size_t index) override{
		return m_columns[index];
	}

	virtual DBDataType getResultType(size_t index) override;

	virtual std::string getInfoString()  override{
		std::ostringstream os;
		os << "SELECT " << m_rows.size();
		return os.str();
	}

	virtual void getResult(size_t index, ResultInfo* pInfo) override;

	virtual void getAllColumns(std::vector<std::string>& columns) override{
		for (auto& s: m_columns) {
			columns.push_back(s);
		}
	}

	virtual int addProjection(ParseNode* pNode) override;

	void addRow(ParseNode* pRow);

private:
	uint64_t m_iCurrent;
	std::vector<std::string> m_columns;
	std::vector<ParseNode*> m_rows;
};
