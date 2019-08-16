#include "PgClient.h"
#include <cassert>
#include <unistd.h>
#include <chrono>

#include <signal.h>
#include <sstream>
#include <vector>

#include <glog/logging.h>

#include "common/ParseException.h"
#include "common/MetaConfig.h"
#include "execution/ExecutionException.h"
#include "execution/ExecutionPlan.h"
#include "execution/ParseTools.h"

namespace {
constexpr int32_t AUTH_REQ_OK = 0; /* User is authenticated  */
constexpr int32_t AUTH_REQ_PASSWORD = 3; /* Password */

constexpr char PG_DIAG_SEVERITY = 'S';
constexpr char PG_DIAG_SQLSTATE = 'C';
constexpr char PG_DIAG_MESSAGE_PRIMARY = 'M';
constexpr char PG_DIAG_MESSAGE_DETAIL = 'D';
constexpr char PG_DIAG_MESSAGE_HINT = 'H';
constexpr char PG_DIAG_STATEMENT_POSITION = 'P';
constexpr char PG_DIAG_INTERNAL_POSITION = 'p';
constexpr char PG_DIAG_INTERNAL_QUERY = 'q';
constexpr char PG_DIAG_CONTEXT = 'W';
constexpr char PG_DIAG_SOURCE_FILE = 'F';
constexpr char PG_DIAG_SOURCE_LINE = 'L';
constexpr char PG_DIAG_SOURCE_FUNCTION = 'R';

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;

class EmptyResult: public ExecutionPlan {
public:
	EmptyResult() :
			ExecutionPlan(PlanType::Other) {
	}

	virtual void explain(std::vector<std::string>& rows) override {
		rows.push_back("Empty");
	}

	virtual std::string getInfoString() override {
		return "SELECT 0";
	}
};
}

PgClient::PgClient(WorkThreadInfo* pInfo) :
		m_receiver(pInfo->m_iAcceptFd), m_sender(pInfo->m_iAcceptFd), m_pWorker(
				pInfo), m_iSendTime(0), m_iParamNum(0){
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
	DLOG(INFO) << "sync";
	m_sender.prepare('Z');
	m_sender.addChar('I');
	m_sender.commit();
	m_sender.flush();
}

void PgClient::handleQuery() {

	auto sql = m_receiver.getNextString();

	DLOG(INFO) << "Q:"<< sql;
	createPlan(sql);

	describeColumn(m_pPlan.get());
	handleExecute();
}

void PgClient::handleParse() {
	auto sStmt = m_receiver.getNextString(); //statement name
	auto sql = m_receiver.getNextString();

	DLOG(INFO)<< "STMT:" <<sStmt<<", SQL:"<< sql;

	createPlan(sql);

	m_iParamNum = m_receiver.getNextShort();

	m_sender.prepare('1');
	m_sender.commit();
}

void PgClient::handleBind() {
	if (m_pPlan.get() == nullptr)
		return;

	auto portal = m_receiver.getNextString();
	auto stmt = m_receiver.getNextString();

	DLOG(INFO)<< "B:portal "<<portal <<" ,stmt "<<stmt;

	int num = m_receiver.getNextShort();
	if (num != m_iParamNum) {
		std::ostringstream os;
		os << "Parameter format number unmatch!, expect "<< m_iParamNum << ", actual " << num;
		throw new ParseException(os.str());
	}
	for (int i = 0; i < m_iParamNum; ++i) {
		if (m_receiver.getNextShort() != PARAM_TEXT_MODE) {
			throw new ParseException(
					"Only text mode bind parameter is supported!");
		}
	}

	num = m_receiver.getNextShort();
	if (num != m_iParamNum) {
		std::ostringstream os;
		os << "Parameter number unmatch!, expect "<< m_iParamNum << ", actual " << num;
		throw new ParseException(os.str());
	}
	if (m_iParamNum > 0) {
		throw new ParseException("Bind Param is not supported!");
	}
	m_sender.prepare('2');
	m_sender.commit();
}

void PgClient::handleDescribe() {
	if (m_pPlan.get() == nullptr)
		return;
	int type = m_receiver.getNextByte();
	auto sName = m_receiver.getNextString();

	DLOG(INFO)<< "D:type "<<type << ",name "<< sName;

	describeColumn(m_pPlan.get());

}

void PgClient::handleExecute() {
	if (m_pPlan.get() == nullptr)
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

	auto sInfo = m_pPlan->getInfoString();

	m_pPlan->end();
	DLOG(INFO)<< "Execute result:" << sInfo;

	m_sender.prepare('C');
	m_sender.addString(sInfo); //data len
	m_sender.commit();
	m_pWorker->m_pPlan = nullptr;
}

void PgClient::handleException(Exception* pe) {
	m_sender.prepare('E');
	m_sender.addByte(PG_DIAG_SEVERITY);
	m_sender.addString("ERROR");
	m_sender.addByte(PG_DIAG_SQLSTATE);
	m_sender.addString("00000");
	m_sender.addByte(PG_DIAG_MESSAGE_PRIMARY);
	m_sender.addString(pe->what());
	LOG(ERROR) << "Error: " << pe->what();

	if (pe->getLine() >= 0) {
		m_sender.addByte(PG_DIAG_STATEMENT_POSITION);
		m_sender.addString(std::to_string(pe->getStartPos()));
	}
	m_sender.addByte('\0');
	delete pe;

	m_sender.commit();
	m_pWorker->clearPlan();
	m_pPlan.reset(nullptr);
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
	m_sender.addString("server_encoding");
	m_sender.addString("UTF8");
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addString("client_encoding");
	m_sender.addString("UTF8");
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addString("server_version");
	m_sender.addString("9.0.4");
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
			DLOG(INFO)<< "Client Terminate!";
			break;
		}
#ifndef NO_TIMEING
		start = std::chrono::steady_clock::now();
#endif
		MessageHandler handler = m_handler[qtype];
		if (handler == nullptr) {
			std::ostringstream os;
			os <<"Unable to handler message "<< qtype;
			throw new IOException(os.str());
		}

		try {
			(this->*handler)();
		} catch (Exception* pe) {
			handleException(pe);
		}
		m_pWorker->m_pPlan = nullptr;
		if (qtype == 'Q')
			handleSync();

#ifndef NO_TIMEING
		auto end = std::chrono::steady_clock::now();

		m_pWorker->m_iClientTime += std::chrono::duration_cast
				< std::chrono::microseconds > (end - start).count();
#endif
	} //while
}

void PgClient::createPlan(const std::string_view sql) {
	m_pPlan.reset(nullptr);
	if (sql.find("DEALLOCATE") == 0) {
		m_pPlan.reset(new EmptyResult());
		DLOG(INFO) << sql;
	} else if (sql.find("SET ") == 0) {
		m_pPlan.reset(new EmptyResult());
		DLOG(INFO) << sql;
	} else {
		m_pWorker->parse(sql);
		m_pPlan.reset(m_pWorker->resolve());
		if (m_pPlan.get() == nullptr) {
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
		auto sName = pPlan->getProjectionName(i);

		switch (pPlan->getResultType(i)) {
		case DBDataType::BYTES:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Bytea, -1);
			break;
		case DBDataType::INT8:
		case DBDataType::INT16:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int16, 2);
			break;
		case DBDataType::INT32:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int32, 4);
			break;
		case DBDataType::INT64:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int64, 8);
			break;

		case DBDataType::STRING:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Varchar, -1);
			break;
		case DBDataType::DATETIME:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1);
			break;
		case DBDataType::DATE:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1);
			break;
		case DBDataType::DOUBLE:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Double, -1);
			break;
		default:
			LOG(ERROR) << "Unknown type " << (int)pPlan->getResultType(i);
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

		if (info.m_bNull) {
			m_sender.addInt(-1);
			continue;
		}
		std::ostringstream os;

		switch (type) {
		case DBDataType::INT8:
		case DBDataType::INT32:
		case DBDataType::INT64:
		case DBDataType::INT16:
			m_sender.addStringAndLength(std::to_string(info.m_lResult));
			break;
		case DBDataType::BYTES: {
			os << "\\x";
			std::string s = "\\x";
			for (auto& c: info.m_sResult) {
				os<< std::hex << c;
			}
			m_sender.addStringAndLength(os.str());
			break;
		}
		case DBDataType::STRING:
			m_sender.addStringAndLength(info.m_sResult);
			break;
		case DBDataType::DATE: {
			time_t time = info.m_time.tv_sec;
			struct tm* pToday = localtime(&time);
			if (pToday == nullptr) {
				LOG(ERROR) << "Failed to get localtime "<< (int ) time;
				m_sender.addInt(0);
			} else {
				char szBuf[100];
				strftime(szBuf, 30, "%Y-%m-%d", pToday);
				m_sender.addStringAndLength(szBuf);
			}
			break;
		}
		case DBDataType::DATETIME: {
			time_t time = info.m_time.tv_sec;
			struct tm* pToday = localtime(&time);
			if (pToday == nullptr) {
				LOG(ERROR) << "Failed to get localtime "<< (int ) time;
				m_sender.addInt(0);
			} else {
				char szBuf[100];
				strftime(szBuf, 30, "%Y-%m-%d %H:%M:%S", pToday);
				m_sender.addStringAndLength(szBuf);
			}
			break;
		}
		case DBDataType::DOUBLE: {
			m_sender.addStringAndLength(std::to_string(info.m_dResult));
			break;
		}
		default:
			assert(0);
			break;
		}; //switch
	} //for

	m_sender.commit();
}
