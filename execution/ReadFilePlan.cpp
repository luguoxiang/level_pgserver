#include "execution/ReadFilePlan.h"
#include "execution/ExecutionException.h"
#include <string.h>

void ReadFilePlan::explain(std::vector<std::string>& rows) {
	char buf[100];
	snprintf(buf, 100, "ReadFile %s, seperator %d", m_pszPath, m_seperator[0]);
	rows.push_back(buf);
}

ReadFilePlan::~ReadFilePlan() {
	if (m_pHandle != nullptr) {
		fclose(m_pHandle);
		m_pHandle = nullptr;
	}
}

int ReadFilePlan::addProjection(ParseNode* pNode) {
	assert(pNode);
	if (pNode->m_iType != NAME_NODE)
		return -1;
	for (size_t i = 0; i < m_columns.size(); ++i) {
		if (strcmp(m_columns[i]->getName(), pNode->m_pszValue) == 0) {
			return i;
		}
	}
	return -1;
}

void ReadFilePlan::begin() {
	m_bCancel = false;
	m_pHandle = fopen(m_pszPath, "r");
	if (m_pHandle == nullptr) {
		char msg[200];
		snprintf(msg, 200, "File %s does not exists!", m_pszPath);
		throw new ExecutionException(msg, false);
	}
}

bool ReadFilePlan::next() {
	if (m_bCancel)
		return false;

	size_t iSize = 4096;
	char* pszBuf = m_szBuf;
	int ret = getline(&pszBuf, &iSize, m_pHandle);
	if (ret == 4096) {
		throw new ExecutionException("input line is too long!", false);
	}
	if (ret < 0)
		return false;

	if (m_szBuf[ret - 1] == '\n')
		m_szBuf[ret - 1] = '\0';
	char *pszLast = nullptr;
	for (int i = 0; i < m_columns.size(); ++i) {
		const char* pszValue = strtok_r(pszLast == nullptr ? m_szBuf : nullptr,
				m_seperator, &pszLast);
		if (pszValue == nullptr) {
			char msg[200];
			snprintf(msg, 200, "Missing values at line %lld!", m_iRowCount + 1);
			throw new ExecutionException(msg, false);
		}
		m_result[i].m_bNull = false;

		switch (m_columns[i]->m_type) {
		case TYPE_INT8:
		case TYPE_INT16:
		case TYPE_INT32:
		case TYPE_INT64:
			m_result[i].m_value.m_lResult = atoll(pszValue);
			break;
		case TYPE_STRING:
			m_result[i].m_value.m_pszResult = pszValue;
			m_result[i].m_len = strlen(pszValue);
			break;
		case TYPE_DATETIME:
		case TYPE_DATE: {
			int64_t iValue = parseTime(pszValue);
			if (iValue == 0) {
				char msg[200];
				snprintf(msg, 200, "Wrong Time Format:%s", pszValue);
				throw new ExecutionException(msg, false);
			}
			struct timeval time;
			time.tv_sec = (iValue / 1000000);
			time.tv_usec = (iValue % 1000000);
			m_result[i].m_value.m_time = time;
			break;
		}
		case TYPE_DOUBLE:
			m_result[i].m_value.m_dResult = atof(pszValue);
			break;
		default:
			break;
		}
	}
	++m_iRowCount;
	return true;

}

void ReadFilePlan::end() {
	if (m_pHandle != nullptr) {
		fclose(m_pHandle);
		m_pHandle = nullptr;
	}
}

