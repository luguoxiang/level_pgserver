#pragma once

#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"

/**
 * SortPlan support an order named Any, normally it is same with Ascend,
 * But it can be upgraded to Ascend or Descend. It is used in following situation:
 * select a from (select * from t) group by a order by a desc;
 * group by will generate a SortPlan so that it can do sort group-by.
 * Because this sql has a 'order by a desc', it better to sort by desc order,
 * But BuildGroupBy plan is called before BuildSortPlan, it has no idea about the latter sort 
 * requirement. At this time, Any order take effect:
 * GroupBy Plan can generate a SortPlan which sort a by order Any.
 * later, when BuilSortPlan query SortPlan::ensureSortOrder,
 * Any order can be upgrated to Descend order.
 */
enum class SortOrder {
	Ascend, Descend, Any
};

class SortPlan: public ExecutionPlan {
	struct SortProjection {
		size_t m_iSubIndex;
		std::string m_sName;
	};
public:
	SortPlan(ExecutionPlan* pPlan);
	virtual ~SortPlan() {end();}

	virtual void explain(std::vector<std::string>& rows)override {
		m_pPlan->explain(rows);
		std::string s = "Sort(";
		for (size_t i = 0; i < m_sort.size(); ++i) {
			s.append(m_sort[i].m_sColumn);
			s.append(" ");
			switch (m_sort[i].m_order) {
			case SortOrder::Ascend:
			case SortOrder::Any:
				s.append("ascend");
				break;
			case SortOrder::Descend:
				s.append("descend");
				break;
			default:
				assert(0);
				break;
			}
			if (i == m_sort.size() - 1)
				s.append(") ");
			else
				s.append(", ");
		}
		s.append("project:");
		for (size_t i = 0; i < m_proj.size(); ++i) {
			s.append(m_proj[i].m_sName);
			if (i < m_proj.size() - 1)
				s.append(", ");
		}

		rows.push_back(s);
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int getResultColumns()override {
		return m_proj.size();
	}

	virtual std::string getProjectionName(size_t index) override{
		return m_proj[index].m_sName;
	}

	virtual DBDataType getResultType(size_t index) override {
		return m_pPlan->getResultType(m_proj[index].m_iSubIndex);
	}

	virtual std::string getInfoString() override{
		return m_pPlan->getInfoString();
	}

	virtual void getResult(size_t index, ResultInfo* pInfo) override;

	virtual void getAllColumns(std::vector<std::string>& columns)  override{
		return m_pPlan->getAllColumns(columns);
	}

	virtual int addProjection(ParseNode* pNode) override;

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder)override;

	void addSortSpecification(ParseNode* pNode, SortOrder order);
private:
	struct SortSpec {
		size_t m_iIndex;
		size_t m_iSubIndex;
		std::string m_sColumn;
		SortOrder m_order;
		DBDataType m_type;
	};

	std::unique_ptr<ExecutionPlan> m_pPlan;

	using RowInfo = std::vector<ResultInfo>;
	std::vector<RowInfo*> m_rows;
	std::vector<SortProjection> m_proj;
	std::vector<SortSpec> m_sort;
	int m_iCurrent;
};

