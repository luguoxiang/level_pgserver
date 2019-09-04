#pragma once

#include <string>
#include <exception>
#include "common/ParseNode.h"
#include "common/ParseException.h"

namespace Tools {
inline bool case_equals(const std::string_view a, const std::string_view b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
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
		PARSE_ERROR("Wrong bind data length ", sValue.length());
		return 0;
	}
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
		PARSE_ERROR("Wrong bind data length ", sValue.length());
		return 0;
	}
}




inline bool isRowKeyNode(const ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME && pNode->getString() == "_rowkey";
}


}
