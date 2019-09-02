#pragma once

#include <string>
#include <exception>
#include "common/ParseNode.h"
#include "execution/WorkThreadInfo.h"
#include "execution/ExecutionPlan.h"
#include "execution/ExecutionException.h"

namespace Tools {
inline bool case_equals(const std::string_view a, const std::string_view b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

inline int64_t toInt(const std::string_view s)
{
	try{
		std::string ss(s.data(), s.length());
		return std::stoi(ss);
	} catch(std::invalid_argument& e) {
		EXECUTION_ERROR("Invalid integer value: ", s);
	} catch (std::out_of_range& e) {
		EXECUTION_ERROR("Value out of range: ", s);
	}
	return 0;
}

inline int64_t binaryToInt(std::string_view sValue) {
	switch(sValue.length()) {
	case 2:
		return ntohs(*(int16_t*) sValue.data());
	case 4:
		return ntohl(*(int32_t*) sValue.data());
	case 8: {
		int32_t hiword = ntohl(*(int32_t*)sValue.data());
		int32_t loword = ntohl(* (int32_t*)(sValue.data() + 4));
		return (static_cast<int64_t>(hiword) << 32) | loword;
	}
	default:
		EXECUTION_ERROR("Wrong bind data length ", sValue.length());
		return 0;
	}
}

inline double toDouble(const std::string_view s)
{
	try{
		std::string ss(s.data(), s.length());
		return std::stod(ss);
	} catch(std::invalid_argument& e) {
		EXECUTION_ERROR("Invalid float value: ", s);
	} catch (std::out_of_range& e) {
		EXECUTION_ERROR("Value out of range: ", s);
	}
	return 0;
}

inline double binaryToDouble(std::string_view sValue) {
	switch(sValue.length()) {
	case 4: {
		int32_t data = ntohl(*(int32_t*)sValue.data());
	    return *reinterpret_cast<float*>(&data);}
	case 8:{
		int32_t hiword = ntohl(*(int32_t*)sValue.data());
		int32_t loword = ntohl(* (int32_t*)(sValue.data() + 4));
		int64_t data = (static_cast<int64_t>(hiword) << 32) | loword;
	    return *reinterpret_cast<double *>(&data);}
	default:
		EXECUTION_ERROR("Wrong bind data length ", sValue.length());
		return 0;
	}
}




inline bool isRowKeyNode(const ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME && pNode->m_sValue == "_rowkey";
}



inline std::string escapeString(const char* pszSrc) {

	std::string s;
	s.append("'");
	for (int64_t i = 0;; ++i) {
		char c = pszSrc[i];
		if (c == '\0') {
			break;
		}
		switch (c) {
		case '\'':
			s.append("\\'");
			break;
		case '\n':
			s.append("\\n");
			break;
		case '\t':
			s.append("\\t");
			break;
		case '\r':
			s.append("\\r");
			break;
		default:
			s += c;
			break;
		}
	}
	s.append("'");

	return s;
}


}
