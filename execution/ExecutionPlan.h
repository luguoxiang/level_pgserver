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
		Other,
};


class ExecutionPlan {
public:

	ExecutionPlan(PlanType type);
	virtual ~ExecutionPlan();

	PlanType getType() {
		return m_type;
	}

	virtual void explain(std::vector<std::string>& rows) = 0;

	/*
	 * To execute this plan, client must use following pattern
	 *
	 * ExecutionPlan* pPlan =  ...
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

	//should call children plan's cancel which make leaf plan throw cancel exception
	virtual void cancel() = 0;

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
			bool* pOrder) = 0;
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
	virtual void getResult(size_t index, ExecutionResult& result) = 0;

private:
	PlanType m_type;
};

