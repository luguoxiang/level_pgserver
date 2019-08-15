#pragma once

#include <string>
#include <stdio.h>
#include "common/SqlParser.tab.h"
#include "common/ParseNode.h"
#include "execution/WorkThreadInfo.h"
#include "execution/ExecutionPlan.h"



#define EXEC_ERROR(msg, append) \
	Log::getLogger().log(LogLevel::ERROR, __FILE__, __LINE__, msg); \
	throw new ExecutionException(msg, append);

namespace Tools {
inline bool isRowKeyNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& strcasecmp(pNode->m_sValue.c_str(), "_rowkey") == 0;
}

inline bool isTimestampNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& strcasecmp(pNode->m_sValue.c_str(), "_timestamp") == 0;
}

inline bool isRowCountNode(ParseNode* pNode) {
	return pNode->m_type == NodeType::NAME
			&& strcasecmp(pNode->m_sValue.c_str(), "_rowcount") == 0;
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
