#pragma once

#include <string>
#include <vector>
#include <variant>
#include "common/ParseNode.h"
#include "common/ConfigInfo.h"

class SqlRunner;
class ScanColumn;
enum class PlanType {
		Sort,
		Projection,
		GroupBy,
		Limit,
		Const,
		ReadFile,
		Explain,
		LzStQuery,
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
	virtual void begin();
	virtual bool next();
	virtual void end();

	virtual void cancel();

	/*
	 * number of projection column
	 */
	virtual int getResultColumns();

	virtual std::string getProjectionName(size_t index) {
		return nullptr;
	}

	virtual DBDataType getResultType(size_t index) {
		return DBDataType::UNKNOWN;
	}

	/*
	 * Add a projection column and return it's result index.
	 * The returned result index can be used as first param of 
	 * getProjectionName, getResultType and getResult.
	 * If the column could not be projected, -1 is returned.
	 * Note that same column must return same index.
	 */
	virtual int addProjection(ParseNode* pColumn) {
		return -1;
	}

	/*
	 * This is used by 'select *' statement.
	 * return all the columns needed to be project for 'select *'.
	 */
	virtual void getAllColumns(std::vector<std::string>& columns) {
	}

	/*
	 * query whether this plan is sorted on specific columns with specific order
	 * This method is used to avoid useless sort like
	 * select * from (select * ..order by a) order by a;
	 */
	virtual bool ensureSortOrder(size_t iSortIndex, const std::string& sColumn,
			bool* pOrder) {
		return false;
	}
	/*
	 * postgres require each SQL statement return a information string
	 * after execution, the format is like this:
	 * for select, SELECT num, num is the number of result set rows
	 * for update, UPDATE num, num is the number of updated rows
	 * for insert, INSERT 0, num, num is the number of inserted rows
	 */
	virtual std::string getInfoString() = 0;

	struct ResultInfo {
		std::variant<std::string_view, int64_t, double, struct timeval> m_result;
		bool m_bNull; // if true, m_value is invalid

		std::string_view getString() const {return std::get<std::string_view>(m_result);}
		int64_t getInt() const {return std::get<int64_t>(m_result);}
		double getDouble() const {return std::get<double>(m_result);}
		struct timeval getTime() const {return std::get<struct timeval>(m_result);}

		int compare(const ResultInfo& result, DBDataType type) const;

		int compare(const ParseNode* pValue, DBDataType type) const;

		bool add(const ResultInfo& result, DBDataType type);

		bool div(size_t value, DBDataType type);
	};

	/*
	 * return result value into corresponding ResultInfo's union Value fields
	 */
	virtual void getResult(size_t index, ResultInfo* pInfo);

private:
	PlanType m_type;
};

