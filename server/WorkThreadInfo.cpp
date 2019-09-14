#include <cassert>
#include <limits>
#include <absl/strings/match.h>

#include "WorkThreadInfo.h"

#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "common/ParseTools.h"

#include "execution/ExecutionException.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;


WorkThreadInfo::WorkThreadInfo(int iIndex) : m_iIndex(iIndex), m_rewritter(m_result){
	m_bTerminate.store(false);
	m_result = {};
	if (parseInit(&m_result)) {
		PARSE_ERROR("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
}



void WorkThreadInfo::setAcceptFd(int fd) {
	m_iAcceptFd = fd;
}
void WorkThreadInfo::cancel(bool planOnly) {
	LOG(INFO) << "Cancel worker" << m_iIndex;
	m_bTerminate.store(true);

	if(!planOnly) {
		::shutdown(m_iAcceptFd, SHUT_RD);
	}
}

void WorkThreadInfo::parse(const std::string_view sql) {
	if (absl::StartsWithIgnoreCase(sql, "DEALLOCATE")) {
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "SET ")) {
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "BEGIN")) {
		LOG(WARNING) << "Transaction is not supported";
		m_result.m_pResult = nullptr;
	} else if (absl::StartsWithIgnoreCase(sql, "COMMIT")) {
		LOG(WARNING) << "Transaction is not supported";
		m_result.m_pResult = nullptr;
	} else {
		++m_iSqlCount;
		parseSql(&m_result, sql);

		m_result.m_pResult = m_rewritter.rewrite(m_result.m_pResult);
		if (m_result.m_pResult == nullptr) {
			throw ParseException(&m_result);
		}

	}
}






