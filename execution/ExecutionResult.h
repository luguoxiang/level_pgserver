#pragma once

#include <string>
#include <variant>

#include "common/ConfigInfo.h"
#include "execution/ExecutionException.h"

class ExecutionResult {

public:
	void setStringView(std::string_view value) {
		m_result = value;
	}
	void setInt(int64_t value) {
		m_result = value;
	}
	void setDouble(double value) {
		m_result = value;
	}

	bool isNull() const {
		return m_result.index() == 0;
	}

	//Warning, if cache() is not called, return value will only be valid before next pPlan->next() call
	std::string_view getString() const {
		try{
			if (m_result.index() == 2) {
				return std::get < std::string > (m_result);
			} else {
				return std::get < std::string_view > (m_result);
			}
		} catch (const std::bad_variant_access& e) {
			EXECUTION_ERROR("wrong DataType expect string");
		}
	}
	int64_t getInt() const {
		try {
			return std::get < int64_t > (m_result);
		} catch (const std::bad_variant_access& e) {
			EXECUTION_ERROR("wrong DataType expect int");
		}
	}
	double getDouble() const {
		try {
			return std::get<double>(m_result);
		} catch (const std::bad_variant_access& e) {
			EXECUTION_ERROR("wrong DataType expect double");
		}
	}

	int compare(const ExecutionResult& result, DBDataType type) const;

	int compare(const ParseNode* pValue, DBDataType type) const;

	void add(const ExecutionResult& result, DBDataType type);

	void div(size_t value, DBDataType type);

	//extend string view life time
	void cache() {
		if (m_result.index() == 1) {
			std::string_view v = std::get < std::string_view > (m_result);
			m_result = std::string(v.data(), v.length());
		}
	}

private:
	std::variant<std::nullptr_t, //should set to other value later
			std::string_view, std::string, int64_t, double> m_result = nullptr;

};
