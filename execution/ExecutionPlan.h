#pragma once

#include <string>
#include <vector>
#include "common/ParseNode.h"
#include "common/ConfigInfo.h"
#include "ExecutionResult.h"

class SqlRunner;
class ScanColumn;
enum class PlanType {
		Sort,
		Projection,
		GroupBy,
		Limit,
		Const,
		ReadFile,
		Insert,
		Delete,
		Scan,
		Other,
};
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

class ExecutionPlan {
public:

	ExecutionPlan(PlanType type);
	virtual ~ExecutionPlan();

	PlanType getType() {
		return m_type;
	}

	virtual void explain(std::vector<std::string>& rows, size_t depth) = 0;

	/*
	 * To execute this plan, client must use following pattern
	 *
	 * ExecutionPlanPtr pPlan =  ...
	 * pPlan->begin();
	 * while(pPlan->next())
	 * {
	 *	  ...//for select，use getResult fetch result set
	 *
	 * }
	 * pPlan->end();
	 */
	virtual void begin() = 0;
	virtual bool next() = 0;
	virtual void end() = 0;
	/*
	 * number of projection column
	 */
	virtual int getResultColumns()= 0;

	virtual std::string_view getProjectionName(size_t index) = 0;

	virtual DBDataType getResultType(size_t index) = 0;

	/*
	 * Add a projection column and return it's result index.
	 * The returned result index can be used as first param of 
	 * getProjectionName, getResultType and getResult.
	 * If the column could not be projected, -1 is returned.
	 * Note that same column must return same index.
	 */
	virtual int addProjection(const ParseNode* pColumn) = 0;

	/*
	 * This is used by 'select *' statement.
	 * return all the columns needed to be project for 'select *'.
	 */
	virtual void getAllColumns(std::vector<std::string_view>& columns) = 0;

	/*
	 * query whether this plan is sorted on specific columns with specific order
	 * This method is used to avoid useless sort like
	 * select * from (select * ..order by a) order by a;
	 */
	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order) = 0;
	/*
	 * postgres require each SQL statement return a information string
	 * after execution, the format is like this:
	 * for select, SELECT num, num is the number of result set rows
	 * for update, UPDATE num, num is the number of updated rows
	 * for insert, INSERT 0, num, num is the number of inserted rows
	 */
	virtual std::string getInfoString() = 0;

	/*
	 * return result value into corresponding ExecutionResult's union Value fields
	 */
	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type) = 0;

	void checkCancellation();
private:
	PlanType m_type;
};

using ExecutionPlanPtr = std::unique_ptr<ExecutionPlan>;
