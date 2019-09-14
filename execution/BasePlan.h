#pragma once

#include <memory>
#include <atomic>
#include "ExecutionPlan.h"
#include "ExecutionException.h"

class SingleChildPlan : public ExecutionPlan {
public:

	SingleChildPlan(PlanType type, ExecutionPlan* pChild) : ExecutionPlan(type), m_pPlan(pChild) {};

	virtual ~SingleChildPlan() {}

	virtual void explain(std::vector<std::string>& rows, size_t depth) override {
		m_pPlan->explain(rows, depth + 1);
	}

	virtual void begin(const std::atomic_bool& bTerminated) override {
		m_pPlan->begin(bTerminated);
	}
	virtual bool next(const std::atomic_bool& bTerminated) override {
		return m_pPlan->next(bTerminated);
	}
	virtual void end() override {
		m_pPlan->end();
	}

	virtual int getResultColumns()override {
		return m_pPlan->getResultColumns();
	}

	virtual std::string_view getProjectionName(size_t index) override{
		return m_pPlan->getProjectionName(index);
	}

	virtual DBDataType getResultType(size_t index) override{
		return m_pPlan->getResultType(index);
	}

	virtual int addProjection(const ParseNode* pColumn)override {
		return m_pPlan->addProjection(pColumn);
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns)override {
		return m_pPlan->getAllColumns(columns);
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order) override{
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, order);
	}

	virtual std::string getInfoString()override {
		return m_pPlan->getInfoString();
	}

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type) override{
		return m_pPlan->getResult(index, result, type);
	}
protected:
	ExecutionPlanPtr m_pPlan;
};


class LeafPlan : public ExecutionPlan {
public:
	LeafPlan(PlanType type);
	virtual ~LeafPlan() {};

	virtual void explain(std::vector<std::string>& rows, size_t depth) {}

	virtual void begin(const std::atomic_bool& bTerminated) {}
	virtual bool next(const std::atomic_bool& bTerminated) {return false;}
	virtual void end() {}

	virtual int getResultColumns() {return 0;}

	virtual std::string_view getProjectionName(size_t index) {
		return "";
	}

	virtual DBDataType getResultType(size_t index) {
		return DBDataType::UNKNOWN;
	}

	virtual int addProjection(const ParseNode* pColumn) {
		return -1;
	}

	virtual void getAllColumns(std::vector<std::string_view>& columns) {
	}

	virtual bool ensureSortOrder(size_t iSortIndex, const std::string_view& sColumn,
			SortOrder order) {
		return false;
	}

	virtual std::string getInfoString() {
		return "SELECT 0";
	}

	virtual void getResult(size_t index, ExecutionResult& result, DBDataType type) {

	}
};

class EmptyPlan : public LeafPlan {
public:
	EmptyPlan(): LeafPlan(PlanType::Other) {}

	virtual void explain(std::vector<std::string>& rows) {
		rows.push_back("NoOperation");
	}
};

inline void CheckCancellation(const std::atomic_bool& bTerminated) {
	if(bTerminated.load()) {
		EXECUTION_ERROR("canceled");
	}
}

