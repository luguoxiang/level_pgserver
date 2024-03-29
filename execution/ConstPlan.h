#pragma once

#include <absl/strings/str_cat.h>

#include "common/ParseException.h"
#include "execution/BasePlan.h"

class ConstPlan: public LeafPlan {
public:
	ConstPlan() : LeafPlan(PlanType::Const), m_iCurrent(0) {
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth)override;

	virtual void begin(const std::atomic_bool& bTerminated)override {
		assert(!m_rows.empty());
		m_iCurrent = 0;
	}

	virtual bool next(const std::atomic_bool& bTerminated)override {
		CheckCancellation(bTerminated);
		return m_iCurrent++ < m_rows.size();
	}

	/*
	 * number of projection column
	 */
	virtual int getResultColumns() override {
		return m_columns.size();
	}

	virtual std::string_view getProjectionName(size_t index) override{
		return m_columns[index];
	}

	virtual DBDataType getResultType(size_t index) override;

	virtual std::string getInfoString()  override{
		return absl::StrCat("SELECT ", m_rows.size());
	}

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type) override;

	virtual void getAllColumns(std::vector<std::string_view>& columns) override{
		for (auto& s: m_columns) {
			columns.push_back(s);
		}
	}

	virtual int addProjection(const ParseNode* pNode) override;

	void addRow(const ParseNode* pRow);

private:
	uint64_t m_iCurrent;
	std::vector<std::string> m_columns;
	std::vector<const ParseNode*> m_rows;
};
