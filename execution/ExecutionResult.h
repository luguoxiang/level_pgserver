#pragma once

#include <string>
#include <variant>

#include "common/ConfigInfo.h"

class ExecutionResult {

public:
	void setStringView(std::string_view value) { m_result = value;}
	void setInt(int64_t value) { m_result = value;}
	void setDouble(double value) { m_result = value;}
	void setTime(const struct timeval& value) { m_result = value;}
	void setNull() { m_result = nullptr;}

	bool isNull() const {return m_result.index() == 0; }

	//Warning, if cache() is not called, return value will only be valid before next pPlan->next() call
	std::string_view getString() const {
		if (m_result.index() == 2) {
			return std::get<std::string>(m_result);
		} else {
			return std::get<std::string_view>(m_result);
		}
	}
	int64_t getInt() const {return std::get<int64_t>(m_result);}
	double getDouble() const {return std::get<double>(m_result);}
	struct timeval getTime() const {return std::get<struct timeval>(m_result);}

	int compare(const ExecutionResult& result, DBDataType type) const;

	int compare(const ParseNode* pValue, DBDataType type) const;

	bool add(const ExecutionResult& result, DBDataType type);

	bool div(size_t value, DBDataType type);

	//extend string view life time
	void cache() {
		if (m_result.index() == 1) {
			std::string_view v= std::get<std::string_view>(m_result);
			m_result = std::string(v.data(), v.length());
		}
	}
private:
	std::variant<
		std::nullptr_t,
		std::string_view,
		std::string,
		int64_t,
		double,
		struct timeval> m_result = nullptr;
};
