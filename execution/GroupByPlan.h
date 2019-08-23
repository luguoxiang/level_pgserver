#pragma once

#include "execution/BasePlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"

#include <sstream>

enum class FuncType {
	FIRST, SUM, AVG, COUNT, MAX, MIN
};
class GroupByPlan: public SingleChildPlan {
public:
	GroupByPlan(ExecutionPlan* pPlan);

	struct AggrFunc {
		FuncType m_func;
		std::string_view m_sName;
		size_t m_iIndex;
		size_t m_iCount;
		ExecutionResult m_value;
	};

	virtual void explain(std::vector<std::string>& rows) override;

	virtual std::string getInfoString()override {
		return ConcateToString("SELECT ", m_iRows);
	}

	virtual int getResultColumns() override{
		return m_proj.size();
	}


	virtual DBDataType getResultType(size_t index)override {
		assert(index < m_proj.size());
		AggrFunc& func = m_proj[index];
		switch (func.m_func) {
		case FuncType::FIRST:
		case FuncType::MIN:
		case FuncType::MAX:
		case FuncType::SUM:
		case FuncType::AVG:
			return m_pPlan->getResultType(func.m_iIndex);
		case FuncType::COUNT:
			return DBDataType::INT64;
		default:
			assert(0);
			return DBDataType::UNKNOWN;
		}
	}

	virtual void getResult(size_t index, ExecutionResult* pInfo)override {
		assert(index < m_proj.size());
		AggrFunc& func = m_proj[index];
		switch (func.m_func) {
		case FuncType::FIRST:
		case FuncType::MIN:
		case FuncType::MAX:
		case FuncType::SUM:
			*pInfo = func.m_value;
			break;
		case FuncType::COUNT:
			pInfo->setInt(func.m_iCount);
			break;
		case FuncType::AVG:
			*pInfo = func.m_value;
			pInfo->div(m_proj[index].m_iCount,
					m_pPlan->getResultType(func.m_iIndex));
			break;
		default:
			assert(0);
			break;
		}
	}

	virtual int addProjection(const ParseNode* pNode) override;

	virtual void getAllColumns(std::vector<std::string_view>& columns) override{
		for (size_t i = 0; i < m_proj.size(); ++i) {
			columns.push_back(m_proj[i].m_sName);
		}
	}

	virtual std::string_view getProjectionName(size_t index)override {
		return m_proj[index].m_sName;
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	void addGroupByColumn(const ParseNode* pNode) {
		int i = m_pPlan->addProjection(pNode);
		if (i < 0) {
			PARSE_ERROR("Unrecognized sort column ", pNode->m_sExpr);
		}
		m_groupby.push_back(i);
	}

private:
	std::vector<AggrFunc> m_proj;
	std::vector<size_t> m_groupby;
	size_t m_iRows = 0;
	bool m_bHasMore = false;

	std::vector<ExecutionResult> m_last;
	std::vector<DBDataType> m_type;
	std::map<std::string_view, FuncType> m_typeMap;
};

