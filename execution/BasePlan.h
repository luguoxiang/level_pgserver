#pragma once

#include <memory>
#include <atomic>
#include "ExecutionPlan.h"

class SingleChildPlan : public ExecutionPlan {
public:

	SingleChildPlan(PlanType type, ExecutionPlan* pChild);

	virtual ~SingleChildPlan() {}

	virtual void explain(std::vector<std::string>& rows) override {
		m_pPlan->explain(rows);
	}

	virtual void begin() override {
		m_pPlan->begin();
	}
	virtual bool next() override {
		return m_pPlan->next();
	}
	virtual void end() override {
		m_pPlan->end();
	}

	virtual void cancel() override {
		m_pPlan->cancel();
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
			bool* pOrder) override{
		return m_pPlan->ensureSortOrder(iSortIndex, sColumn, pOrder);
	}

	virtual std::string getInfoString()override {
		return m_pPlan->getInfoString();
	}

	virtual void getResult(size_t index, ExecutionResult& result) override{
		return m_pPlan->getResult(index, result);
	}
protected:
	std::unique_ptr<ExecutionPlan> m_pPlan;
};


class LeafPlan : public ExecutionPlan {
public:
	LeafPlan(PlanType type);
	virtual ~LeafPlan() {};

	virtual void explain(std::vector<std::string>& rows) {}

	virtual void begin() {}
	virtual bool next();
	virtual void end() {}

	virtual void cancel() {
		m_bCancel = true;
	}

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
			bool* pOrder) {
		return false;
	}

	virtual std::string getInfoString() {
		return "SELECT 0";
	}

	virtual void getResult(size_t index, ExecutionResult& result) {

	}

private:
	//may be set by another thread
	std::atomic<bool> m_bCancel = false;
};



