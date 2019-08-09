#include "PgClient.h"
#include <cassert>
#include <unistd.h>
#include <chrono>
#define AUTH_REQ_OK             0       /* User is authenticated  */
#define AUTH_REQ_PASSWORD       3       /* Password */

#define PG_DIAG_SEVERITY		'S'
#define PG_DIAG_SQLSTATE		'C'
#define PG_DIAG_MESSAGE_PRIMARY 'M'
#define PG_DIAG_MESSAGE_DETAIL	'D'
#define PG_DIAG_MESSAGE_HINT	'H'
#define PG_DIAG_STATEMENT_POSITION 'P'
#define PG_DIAG_INTERNAL_POSITION 'p'
#define PG_DIAG_INTERNAL_QUERY	'q'
#define PG_DIAG_CONTEXT			'W'
#define PG_DIAG_SOURCE_FILE		'F'
#define PG_DIAG_SOURCE_LINE		'L'
#define PG_DIAG_SOURCE_FUNCTION 'R'

#define PARAM_TEXT_MODE 0
#define PARAM_BINARY_MODE 1

#include <signal.h>
#include <iostream>
#include <vector>

#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "common/Log.h"
#include "execution/ExecutionException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"

namespace {
class EmptyResult: public ExecutionPlan {
public:
	EmptyResult() :
			ExecutionPlan(Other) {
	}

	virtual void explain(std::vector<std::string>& rows) {
		rows.push_back("Empty");
	}

	virtual void getInfoString(char* szBuf, int len) {
		snprintf(szBuf, len, "SELECT 0");
	}
};
}

PgClient::PgClient(WorkThreadInfo* pInfo) :
		m_receiver(pInfo->m_iAcceptFd), m_sender(pInfo->m_iAcceptFd), m_pWorker(
				pInfo), m_iSendTime(0), m_iParamNum(0), m_pszSql("") {
	assert(pInfo->m_iAcceptFd >= 0);

	memset(m_handler, 0, sizeof(m_handler));

	m_handler['S'] = &PgClient::handleSync;

	m_handler['Q'] = &PgClient::handleQuery;

	m_handler['P'] = &PgClient::handleParse;
	m_handler['B'] = &PgClient::handleBind;
	m_handler['D'] = &PgClient::handleDescribe;
	m_handler['E'] = &PgClient::handleExecute;
}

PgClient::~PgClient() {
	::close(m_pWorker->m_iAcceptFd);
}

void PgClient::handleSync() {
	LOG(DEBUG, "sync");
	m_sender.prepare('Z');
	m_sender.addString("I", 1);
	m_sender.commit();
	m_sender.flush();
}

void PgClient::handleQuery() {
	size_t len;
	m_pszSql = m_receiver.getNextString(&len);
	if (len >= POOL_BLOCK_SIZE) {
		throw new ParseException(" Sql length could not exceeds 32KB");
	}
	LOG(DEBUG, "Q:%s", m_pszSql);
	createPlan(m_pszSql, len);

	describeColumn(m_pPlan.get());
	handleExecute();
}

void PgClient::handleParse() {
	size_t len1, len2;
	const char* pszStmt = m_receiver.getNextString(&len1); //statement name
	const char* pszSql = m_receiver.getNextString(&len2);
	if (len1 >= POOL_BLOCK_SIZE || len2 >= POOL_BLOCK_SIZE) {
		throw new ParseException(" Sql length could not exceeds 32KB");
	}

	LOG(DEBUG, "STMT:%d,P:%s", len1, pszSql);

	createPlan(pszSql, len2);

	m_iParamNum = m_receiver.getNextShort();

	m_sender.prepare('1');
	m_sender.commit();
}

void PgClient::handleBind() {
	if (m_pPlan.get() == NULL)
		return;

	size_t len1, len2;
	m_receiver.getNextString(&len1);
	m_receiver.getNextString(&len2);

	LOG(DEBUG, "B:portal %d,stmt %d.", len1, len2);

	int num = m_receiver.getNextShort();
	if (num != m_iParamNum) {
		throw new ParseException(
				"Parameter format number unmatch!, expect %d, actual %d!",
				m_iParamNum, num);
	}
	for (int i = 0; i < m_iParamNum; ++i) {
		if (m_receiver.getNextShort() != PARAM_TEXT_MODE) {
			throw new ParseException(
					"Only text mode bind parameter is supported!");
		}
	}

	num = m_receiver.getNextShort();
	if (num != m_iParamNum) {
		throw new ParseException(
				"Parameter number unmatch, expect %d, actual %d!", m_iParamNum,
				num);
	}
	if (m_iParamNum > 0) {
		throw new ParseException("Bind Param is not supported!");
	}
	m_sender.prepare('2');
	m_sender.commit();
}

void PgClient::handleDescribe() {
	if (m_pPlan.get() == NULL)
		return;
	size_t len;
	int type = m_receiver.getNextByte();
	const char* pszName = m_receiver.getNextString(&len);

	LOG(DEBUG, "D:type %d,name %s.", type, pszName);

	describeColumn(m_pPlan.get());

}

void PgClient::handleExecute() {
	if (m_pPlan.get() == NULL)
		return;

	size_t columnNum = m_pPlan->getResultColumns();

	m_pWorker->m_pPlan = m_pPlan.get();
	m_pPlan->begin();

	while (m_pPlan->next()) {
		if (columnNum == 0)
			continue;
		sendRow(m_pPlan.get());
	} //while
	m_sender.flush();

	char szInfo[100];
	m_pPlan->getInfoString(szInfo, 100);

	m_pPlan->end();
	LOG(DEBUG, "Execute result:%s!", szInfo);

	m_sender.prepare('C');
	m_sender.addString(szInfo, strlen(szInfo) + 1); //data len
	m_sender.commit();
	m_pWorker->m_pPlan = NULL;
}

void PgClient::handleException(Exception* pe) {
	m_sender.prepare('E');
	m_sender.addByte(PG_DIAG_SEVERITY);
	m_sender.addString("ERROR", 6);
	m_sender.addByte(PG_DIAG_SQLSTATE);
	m_sender.addString("00000", 6);
	m_sender.addByte(PG_DIAG_MESSAGE_PRIMARY);
	m_sender.addString(pe->what(), strlen(pe->what()) + 1);
	LOG(ERROR, "Error:%s:%s!", m_pszSql, pe->what());

	if (pe->getLine() >= 0) {
		m_sender.addByte(PG_DIAG_STATEMENT_POSITION);
		char szBuf[10];
		sprintf(szBuf, "%d", pe->getStartPos());
		m_sender.addString(szBuf, strlen(szBuf) + 1);
	}
	m_sender.addByte('\0');
	delete pe;

	m_sender.commit();
	m_pWorker->clearPlan();
	m_pPlan.reset(NULL);
}

void PgClient::run() {
#ifndef NO_TIMEING
	auto start = std::chrono::steady_clock::now();
#endif
	m_receiver.processStartupPacket();

	m_sender.prepare('R');
	m_sender.addInt(AUTH_REQ_OK);
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addString("server_encoding", 16);
	m_sender.addString("UTF8", 5);
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addString("client_encoding", 16);
	m_sender.addString("UTF8", 5);
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addString("server_version", 15);
	m_sender.addString("9.0.4", 6);
	m_sender.commit();

	m_sender.prepare('K');
	m_sender.addInt(m_pWorker->m_iIndex);
	m_sender.addInt(0); //cancel key
	m_sender.commit();

	handleSync();
#ifndef NO_TIMEING
	auto end = std::chrono::steady_clock::now();

	m_pWorker->m_iClientTime += std::chrono::duration_cast
			< std::chrono::microseconds > (end - start).count();
#endif

	while (true) {
		char qtype = m_receiver.readMessage();
		if (qtype == 'X') {
			LOG(DEBUG, "Client Terminate!\n");
			break;
		}
#ifndef NO_TIMEING
		start = std::chrono::steady_clock::now();
#endif
		MessageHandler handler = m_handler[qtype];
		if (handler == NULL) {
			char szBuf[100];
			snprintf(szBuf, 100, "Unable to handler message %c", qtype);
			throw new IOException(szBuf);
		}

		try {
			(this->*handler)();
		} catch (Exception* pe) {
			handleException(pe);
		}
		m_pWorker->m_pPlan = NULL;
		if (qtype == 'Q')
			handleSync();

#ifndef NO_TIMEING
		auto end = std::chrono::steady_clock::now();

		m_pWorker->m_iClientTime += std::chrono::duration_cast
				< std::chrono::microseconds > (end - start).count();
#endif
	} //while
}

void PgClient::createPlan(const char* pszCmd, size_t len) {
	m_pPlan.reset(NULL);
	if (strncasecmp("DEALLOCATE", pszCmd, 10) == 0) {
		m_pPlan.reset(new EmptyResult());
		LOG(DEBUG, "%s", pszCmd);
	} else if (strncasecmp("SET ", pszCmd, 4) == 0) {
		m_pPlan.reset(new EmptyResult());
		LOG(DEBUG, "%s", pszCmd);
	} else {
		m_pWorker->parse(pszCmd, len);
		m_pPlan.reset(m_pWorker->resolve());
		if (m_pPlan.get() == NULL) {
			throw new ParseException("No statement!");
		}
	}
}

void PgClient::describeColumn(ExecutionPlan* pPlan) {
	assert(pPlan);

	size_t columnNum = pPlan->getResultColumns();
	if (columnNum == 0)
		return;

	m_sender.prepare('T');
	m_sender.addShort(columnNum);

	for (size_t i = 0; i < columnNum; ++i) {
		const char* pszName = pPlan->getProjectionName(i);

		switch (pPlan->getResultType(i)) {
		case TYPE_BYTES:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Bytea, -1);
			break;
		case TYPE_INT8:
		case TYPE_INT16:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Int16, 2);
			break;
		case TYPE_INT32:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Int32, 4);
			break;
		case TYPE_INT64:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Int64, 8);
			break;

		case TYPE_STRING:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Varchar,
					-1);
			break;
		case TYPE_DATETIME:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::DateTime,
					-1);
			break;
		case TYPE_DATE:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::DateTime,
					-1);
			break;
		case TYPE_DOUBLE:
			m_sender.addDataTypeMsg(pszName, i + 1, PgMessageSender::Double,
					-1);
			break;
		default:
			LOG(ERROR, "Unknown type %d\n", pPlan->getResultType(i))
			;
			assert(0);
			break;
		}
	}
	m_sender.commit();
	m_sender.flush();
}

void PgClient::sendRow(ExecutionPlan* pPlan) {
	size_t columnNum = pPlan->getResultColumns();
	assert(columnNum > 0);
	m_sender.prepare('D');
	m_sender.addShort(columnNum); //field number
	for (size_t i = 0; i < columnNum; ++i) {
		DBDataType type = pPlan->getResultType(i);

		ExecutionPlan::ResultInfo info;
		try {
			pPlan->getResult(i, &info);
		} catch (...) {
			for (; i < columnNum; ++i)
				m_sender.addInt(-1);
			m_sender.commit();
			m_sender.flush();
			throw;
		}
		char szBuf[100];
		size_t len;
		if (info.m_bNull) {
			m_sender.addInt(-1);
			continue;
		}
		switch (type) {
		case TYPE_INT8:
		case TYPE_INT32:
		case TYPE_INT64:
		case TYPE_INT16:
			len = snprintf(szBuf, 100, "%lld", info.m_value.m_lResult);
			m_sender.addInt(len);
			m_sender.addString(szBuf, len);
			break;
		case TYPE_BYTES: {
			std::string s = "\\x";
			for (int64_t i = 0; i < info.m_len; ++i) {
				char szBuf[10];
				snprintf(szBuf, 10, "%02x",
						(unsigned char) info.m_value.m_pszResult[i]);
				s.append(szBuf);
			}
			m_sender.addInt(s.size());
			m_sender.addString(s.c_str(), s.size());
			break;
		}
		case TYPE_STRING:
			len = info.m_len;
			m_sender.addInt(len);
			m_sender.addString(info.m_value.m_pszResult, len);
			break;
		case TYPE_DATE: {
			time_t time = info.m_value.m_time.tv_sec;
			struct tm* pToday = localtime(&time);
			if (pToday == NULL) {
				LOG(ERROR, "Failed to get localtime %d\n", (int ) time);
				len = 0;
			} else {
				strftime(szBuf, 30, "%Y-%m-%d", pToday);
				len = strlen(szBuf);
			}
			m_sender.addInt(len);
			m_sender.addString(szBuf, len);
			break;
		}
		case TYPE_DATETIME: {
			time_t time = info.m_value.m_time.tv_sec;
			struct tm* pToday = localtime(&time);
			if (pToday == NULL) {
				LOG(ERROR, "Failed to get localtime %d\n", (int ) time);
				len = 0;
			} else {
				strftime(szBuf, 30, "%Y-%m-%d %H:%M:%S", pToday);
				len = strlen(szBuf);
			}
			m_sender.addInt(len);
			m_sender.addString(szBuf, len);
			break;
		}
		case TYPE_DOUBLE: {
			len = snprintf(szBuf, 100, "%f", info.m_value.m_dResult);
			while (len > 0) {
				char c = szBuf[len - 1];
				if (c > '0' && c <= '9')
					break;
				--len;
				szBuf[len] = '\0';
				if (c == '.')
					break;
			}
			m_sender.addInt(len);
			m_sender.addString(szBuf, len);
			break;
		}
		default:
			assert(0);
			break;
		}; //switch
	} //for

	m_sender.commit();
}
