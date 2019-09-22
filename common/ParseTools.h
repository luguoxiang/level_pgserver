#pragma once

#include <string>
#include <exception>

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "wsock32.lib")
#else
#include <netinet/in.h>
#endif

#include <absl/strings/match.h>

#include "common/ParseNode.h"
#include "common/ParseException.h"

namespace Tools {

inline int64_t binaryToInt(std::string_view sValue) {
	switch(sValue.length()) {
	case 1:
		return *sValue.data();
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


constexpr const char* ROWKEY = "_rowkey";

inline bool isRowKeyNode(const ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& absl::EqualsIgnoreCase(pNode->getString(), ROWKEY);
}


}
