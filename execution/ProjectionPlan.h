#pragma once

#include "execution/GroupByPlan.h"
#include <vector>
#include <map>
class ProjectionPlan: public SingleChildPlan {
	struct ProjectionInfo {
		int m_iSubIndex;
		std::string_view m_sName;
		std::string_view m_sRaw;
		const ParseNode* m_pNode;
	};
public:
	ProjectionPlan(ExecutionPlan*pPlan) :
		SingleChildPlan(PlanType::Projection, pPlan){}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override{

		std::ostringstream os;
		os << std::string(depth, '\t');
		os << "Projection ";

		std::string s = "Projection ";
		for (size_t i = 0; i < m_proj.size(); ++i) {
			os << m_proj[i].m_sName<< ", ";
		}
		rows.push_back(os.str());
		SingleChildPlan::explain(rows, depth);
	}

	bool project(const ParseNode* pNode, const std::string_view& sName);

	virtual void getAllColumns(std::vector<std::string_view>& columns) override{
		for (size_t i = 0; i < m_proj.size(); ++i) {
			columns.push_back(m_proj[i].m_sName);
		}
	}

	virtual int addProjection(const ParseNode* pNode)override{
		auto iter = m_map.find(pNode->getString());
		if (iter == m_map.end())
			return -1;
		return iter->second;
	}


	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order) override {
		if (auto iter = m_map.find(sColumn); iter != m_map.end() ) {
			size_t iIndex = iter->second;
			return m_pPlan->ensureSortOrder(iSortIndex, m_proj[iIndex].m_sRaw, order);
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

	virtual DBDataType getResultType(size_t index)override;


	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type)override;

	bool addGroupBy() {
		m_pPlan.reset(new GroupByPlan(m_pPlan.release()));
		for(auto& proj : m_proj) {
			proj.m_iSubIndex = m_pPlan->addProjection(proj.m_pNode);
			if (proj.m_iSubIndex < 0) {
				return false;
			}
		}
		return true;
	}
private:
	std::vector<ProjectionInfo> m_proj;
	std::map<std::string_view, size_t> m_map;
};
