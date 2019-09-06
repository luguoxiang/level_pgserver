#pragma once

#include <string>
#include <variant>
#include <absl/strings/str_cat.h>

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
		return m_result.index() == NULLPTR;
	}

	bool isMaxString() const {
		return m_result.index() == MAX_STRING;
	}

	void setMaxString() {
		m_result = MaxString{};
	}
	//Warning, if cache() is not called, return value will only be valid before next pPlan->next() call
	std::string_view getString() const {
		switch(m_result.index()) {
		case STRING:
			return std::get < std::string > (m_result);
		case STRING_VIEW:
			return std::get < std::string_view > (m_result);
		default:
			EXECUTION_ERROR("wrong DataType expect string, got ", toString());
			return "";
		}
	}
	int64_t getInt() const {
		if (m_result.index() == INT) {
			return std::get < int64_t > (m_result);
		} else {
			EXECUTION_ERROR("wrong DataType expect int, got ", toString());
		}
	}

	double getDouble() const {
		switch(m_result.index()) {
		case INT :
			return std::get<int64_t>(m_result);
		case DOUBLE:
			return std::get<double>(m_result);
		default:
			EXECUTION_ERROR("wrong DataType expect double, got ", toString());
		}
	}

	int compare(const ExecutionResult& result, DBDataType type) const;


	void add(const ExecutionResult& result, DBDataType type);

	void div(size_t value, DBDataType type);

	//extend string view life time
	void cache() {
		if (m_result.index() == STRING_VIEW) {
			std::string_view v = std::get < std::string_view > (m_result);
			m_result = std::string(v.data(), v.length());
		}
	}

	std::string toString() const{
		return std::visit(GetString(), m_result);
	}

private:
	static constexpr size_t NULLPTR = 0;
	static constexpr size_t STRING_VIEW = 1;
	static constexpr size_t STRING = 2;
	static constexpr size_t MAX_STRING = 3;
	static constexpr size_t INT = 4;
	static constexpr size_t DOUBLE = 5;

	struct MaxString {};

	struct GetString
	{
	    std::string operator()(const std::nullptr_t& ) { return "null"; }
	    std::string operator()(const std::string_view& v) { return absl::StrCat("string:", v); }
		std::string operator()(const std::string& v) { return absl::StrCat("string:", v); }
		std::string operator()(const MaxString& v) { return "string_max"; }
		std::string operator()(const int64_t& v) { return absl::StrCat("int:", v); }
		std::string operator()(const double& v) { return absl::StrCat("double:", v); }
	};

	std::variant<std::nullptr_t, //should set to other value later
			std::string_view, std::string, MaxString, int64_t, double> m_result = nullptr;

};
