#pragma once

#include <string>
#include <stdio.h>
#include "common/SqlParser.tab.h"
#include "common/ParseNode.h"
#include "execution/WorkThreadInfo.h"
#include "execution/ExecutionPlan.h"



#define PARSE_ERROR(FORMAT, ...) \
	Log::getLogger().log(Log::ERROR, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__); \
	throw new ParseException(FORMAT, ##__VA_ARGS__);

#define EXEC_ERROR(msg, append) \
	Log::getLogger().log(Log::ERROR, __FILE__, __LINE__, msg); \
	throw new ExecutionException(msg, append);

namespace Tools {
inline bool isRowKeyNode(ParseNode* pNode) {
	return pNode->m_iType == NAME_NODE
			&& strcasecmp(pNode->m_pszValue, "_rowkey") == 0;
}

inline bool isTimestampNode(ParseNode* pNode) {
	return pNode->m_iType == NAME_NODE
			&& strcasecmp(pNode->m_pszValue, "_timestamp") == 0;
}

inline bool isRowCountNode(ParseNode* pNode) {
	return pNode->m_iType == NAME_NODE
			&& strcasecmp(pNode->m_pszValue, "_rowcount") == 0;
}

inline const char* byteToString(const char* p, size_t iLen) {
	if (p == nullptr) {
		return "null";
	}
	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	char* pszRet = pInfo->alloc(iLen * 2 + 3);
	snprintf(pszRet, 3, "0x");
	for (size_t i = 0; i < iLen; ++i) {
		snprintf(pszRet + 2 + i * 2, 3, "%02x", (unsigned char) p[i]);
	}
	return pszRet;
}

inline const char* escapeString(const char* pszSrc, int64_t iLen) {

	std::string s;
	s.append("'");
	for (int64_t i = 0; i < iLen; ++i) {
		char c = pszSrc[i];
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

	WorkThreadInfo* pInfo = WorkThreadInfo::m_pWorkThreadInfo;
	assert(pInfo);
	return pInfo->memdup(s.c_str(), s.size() + 1);
}

inline bool hasRowKey(ParseNode* pPredicate) {
	if (isRowKeyNode(pPredicate))
		return true;

	if (pPredicate->m_iChildNum == 0)
		return false;

	for (size_t i = 0; i < pPredicate->m_iChildNum; ++i) {
		if (hasRowKey(pPredicate->m_children[i]))
			return true;
	}
	return false;
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
