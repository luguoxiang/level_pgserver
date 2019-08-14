#pragma once

#include "execution/GroupByPlan.h"
#include "execution/ParseTools.h"
#include <vector>
#include <map>
class ProjectionPlan: public ExecutionPlan {
	struct ProjectionInfo {
		size_t m_iSubIndex;
		std::string m_sName;
		std::string m_sRaw;
	};
public:
	ProjectionPlan(ExecutionPlan* pPlan) :
			ExecutionPlan(PlanType::Projection), m_pPlan(pPlan) {
		assert(pPlan);
	}

	virtual void explain(std::vector<std::string>& rows) override{
		m_pPlan->explain(rows);
		std::string s = "Projection ";
		for (size_t i = 0; i < m_proj.size(); ++i) {
			s.append(m_proj[i].m_sName);
			s.append(", ");
		}
		rows.push_back(s);
	}

	bool project(ParseNode* pNode, const std::string& sName);

	virtual void getAllColumns(std::vector<std::string>& columns) override{
		for (size_t i = 0; i < m_proj.size(); ++i) {
			columns.push_back(m_proj[i].m_sName);
		}
	}

	virtual int addProjection(ParseNode* pNode)override{
		auto iter = m_map.find(pNode->m_sExpr);
		if (iter == m_map.end())
			return -1;
		return iter->second;
	}

	virtual void begin()override {
		m_pPlan->begin();
	}
	virtual bool next() override{
		return m_pPlan->next();
	}

	virtual void end()override {
		return m_pPlan->end();
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) override {
		auto iter = m_map.find(sColumn);
		if (iter == m_map.end())
			return false;
		size_t iIndex = iter->second;
		return m_pPlan->ensureSortOrder(iSortIndex, m_proj[iIndex].m_sRaw, pOrder);
	}
	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_proj.size();
	}

	virtual std::string getProjectionName(size_t index) override{
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_proj[index].m_sName;
	}

	virtual DBDataType getResultType(size_t index)override {
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResultType(iSubIndex);
	}

	virtual std::string getInfoString() override{
		return m_pPlan->getInfoString();
	}

	virtual void getResult(size_t index, ResultInfo* pInfo)override {
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResult(iSubIndex, pInfo);
	}

	void addGroupBy() {
		auto pGroupBy = new GroupByPlan(m_pPlan.release());
		m_pPlan.reset(pGroupBy);
	}
private:
	std::vector<ProjectionInfo> m_proj;
	std::map<std::string, size_t> m_map;
	std::unique_ptr<ExecutionPlan> m_pPlan;
};
