#pragma once

#include "execution/GroupByPlan.h"
#include "execution/ParseTools.h"
#include <vector>
#include <map>
class ProjectionPlan: public SingleChildPlan {
	struct ProjectionInfo {
		size_t m_iSubIndex;
		std::string_view m_sName;
		std::string_view m_sRaw;
		const ParseNode* pNode;
	};
public:
	ProjectionPlan(ExecutionPlan* pPlan) :
		SingleChildPlan(PlanType::Projection, pPlan){}

	virtual void explain(std::vector<std::string>& rows) override{
		SingleChildPlan::explain(rows);
		std::string s = "Projection ";
		for (size_t i = 0; i < m_proj.size(); ++i) {
			s.append(m_proj[i].m_sName);
			s.append(", ");
		}
		rows.push_back(s);
	}

	bool project(const ParseNode* pNode, const std::string_view& sName);

	virtual void getAllColumns(std::vector<std::string_view>& columns) override{
		for (size_t i = 0; i < m_proj.size(); ++i) {
			columns.push_back(m_proj[i].m_sName);
		}
	}

	virtual int addProjection(const ParseNode* pNode)override{
		auto iter = m_map.find(pNode->m_sValue);
		if (iter == m_map.end())
			return -1;
		return iter->second;
	}


	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			bool* pOrder) override {
		if (auto iter = m_map.find(sColumn); iter != m_map.end() ) {
			size_t iIndex = iter->second;
			return m_pPlan->ensureSortOrder(iSortIndex, m_proj[iIndex].m_sRaw, pOrder);
		} else {
			return false;
		}

	}
	/*
	 * number of projection column
	 */
	virtual int getResultColumns()override {
		return m_proj.size();
	}

	virtual std::string_view getProjectionName(size_t index) override{
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_proj[index].m_sName;
	}

	virtual DBDataType getResultType(size_t index)override {
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResultType(iSubIndex);
	}


	virtual void getResult(size_t index, ExecutionResult& result)override {
		assert(index < m_proj.size());
		size_t iSubIndex = m_proj[index].m_iSubIndex;
		return m_pPlan->getResult(iSubIndex, result);
	}

	bool addGroupBy() {
		auto pGroupBy = new GroupByPlan(m_pPlan.release());
		for(auto& proj : m_proj) {
			proj.m_iSubIndex = pGroupBy->addProjection(proj.pNode);
			if (proj.m_iSubIndex < 0) {
				return false;
			}
		}
		m_pPlan.reset(pGroupBy);
		return true;
	}
private:
	std::vector<ProjectionInfo> m_proj;
	std::map<std::string_view, size_t> m_map;
};
