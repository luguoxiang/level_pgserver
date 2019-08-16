#pragma once

#include <string>
#include <stdio.h>
#include "common/SqlParser.tab.h"
#include "common/ParseNode.h"
#include "execution/WorkThreadInfo.h"
#include "execution/ExecutionPlan.h"


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
	int64_t result = 0;
    for(auto c: s) {
    	result = result * 10 + (c - '0');
    }
    return result;
}


inline bool isRowKeyNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& case_equals(pNode->m_sValue, "_rowkey");
}

inline bool isTimestampNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& case_equals(pNode->m_sValue, "_timestamp");
}

inline bool isRowCountNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& case_equals(pNode->m_sValue, "_rowcount");
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


#define CHECK_ERROR(err,msg) \
		if (OB_ERR_SUCCESS != err) \
		{\
			LOG(ERROR, msg);\
			throw new ExecutionException(msg, true);\
		}\

inline void pushPlan(ExecutionPlan* pPlan) {
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	pInfo->pushPlan(pPlan);
}

inline ExecutionPlan* popPlan() {
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	return pInfo->popPlan();
}
}
