#pragma once

#include "execution/ExecutionBuffer.h"
#include "execution/BasePlan.h"
#include "common/ParseException.h"


class SortPlan: public SingleChildPlan {
	struct SortProjection {
		size_t m_iSubIndex;
		std::string_view m_sName;
	};
public:
	SortPlan(ExecutionPlan* pPlan);
	virtual ~SortPlan() {}

	virtual void explain(std::vector<std::string>& rows, size_t depth)override {

		std::ostringstream os;
		os << std::string(depth, '\t');
		os << "Sort(";

		std::string s = "Sort(";
		for (size_t i = 0; i < m_sort.size(); ++i) {
			os << (m_sort[i].m_sColumn);
			os << (" ");
			switch (m_sort[i].m_order) {
			case SortOrder::Ascend:
			case SortOrder::Any:
				os << ("ascend");
				break;
			case SortOrder::Descend:
				os << ("descend");
				break;
			default:
				assert(0);
				break;
			}
			if (i == m_sort.size() - 1)
				os << (") ");
			else
				os << (", ");
		}
		os << ("project:");
		for (size_t i = 0; i < m_proj.size(); ++i) {
			os << (m_proj[i].m_sName);
			if (i < m_proj.size() - 1)
				os << (", ");
		}

		rows.push_back(os.str());
		m_pPlan->explain(rows, depth);
	}

	virtual void begin() override;
	virtual bool next() override;
	virtual void end() override;

	virtual int getResultColumns()override {
		return m_proj.size();
	}

	virtual std::string_view getProjectionName(size_t index) override{
		return m_proj[index].m_sName;
	}

	virtual DBDataType getResultType(size_t index) override {
		return m_pPlan->getResultType(m_proj[index].m_iSubIndex);
	}


	virtual void getResult(size_t index, ExecutionResult& result) override;

	virtual int addProjection(const ParseNode* pNode) override;

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order)override;

	void addSortSpecification(const ParseNode* pNode, SortOrder order);

private:
	struct SortSpec {
		size_t m_iIndex;
		size_t m_iSubIndex;
		std::string m_sColumn;
		SortOrder m_order;
	};


	std::vector<const std::byte*> m_rows;
	std::vector<SortProjection> m_proj;
	std::vector<SortSpec> m_sort;

	std::vector<DBDataType> m_types;
	int m_iCurrent = 0;

	std::unique_ptr<ExecutionBuffer> m_pBuffer;
};

