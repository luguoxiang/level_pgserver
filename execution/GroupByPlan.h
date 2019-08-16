#pragma once

#include "ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"

#include <sstream>

enum class FuncType {
	FIRST, SUM, AVG, COUNT, MAX, MIN
};
class GroupByPlan: public ExecutionPlan {
public:
	GroupByPlan(ExecutionPlan* pPlan);

	struct AggrFunc {
		FuncType m_func;
		std::string m_sName;
		size_t m_iIndex;
		size_t m_iCount;
		ResultInfo m_value;
	};

	virtual void explain(std::vector<std::string>& rows) override;

	virtual std::string getInfoString()override {
		std::ostringstream os;
		os << "SELECT " << m_iRows;
		return os.str();
	}

	virtual int getResultColumns() override{
		return m_proj.size();
	}

	virtual DBDataType getResultType(size_t index)override {
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

	virtual void getResult(size_t index, ResultInfo* pInfo)override {
		AggrFunc& func = m_proj[index];
		switch (func.m_func) {
		case FuncType::FIRST:
		case FuncType::MIN:
		case FuncType::MAX:
		case FuncType::SUM:
			*pInfo = func.m_value;
			break;
		case FuncType::COUNT:
			pInfo->m_bNull = false;
			pInfo->m_result = int64_t(func.m_iCount);
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

	virtual int addProjection(ParseNode* pNode) override;

	virtual void getAllColumns(std::vector<std::string>& columns) override{
		for (size_t i = 0; i < m_proj.size(); ++i) {
			columns.push_back(m_proj[i].m_sName);
		}
	}

	virtual std::string getProjectionName(size_t index)override {
		return m_proj[index].m_sName;
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) override{
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, pOrder);
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	void addGroupByColumn(ParseNode* pNode) {
		int i = m_pPlan->addProjection(pNode);
		if (i < 0) {
			std::ostringstream os;
			os << "Unrecognized sort column " << pNode->m_sExpr;
			throw new ParseException(os.str());
		}
		m_groupby.push_back(i);
	}

private:
	std::unique_ptr<ExecutionPlan> m_pPlan;
	std::vector<AggrFunc> m_proj;
	std::vector<size_t> m_groupby;
	size_t m_iRows = 0;
	bool m_bHasMore = false;

	std::vector<ResultInfo> m_last;
	std::vector<DBDataType> m_type;
	std::map<std::string, FuncType> m_typeMap;
};

