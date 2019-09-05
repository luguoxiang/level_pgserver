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
#include "execution/WorkThreadInfo.h"

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


}

PgClient::PgClient(WorkThreadInfo* pInfo, std::atomic_bool& bGlobalTerminate) :
		m_receiver(pInfo->getAcceptFd()),
		m_sender(pInfo->getAcceptFd()),
		m_bGlobalTerminate(bGlobalTerminate),
		m_pWorker(pInfo){
	assert(pInfo->getAcceptFd() >= 0);

	memset(m_handler, 0, sizeof(m_handler));

	m_handler['S'] = &PgClient::handleSync;

	m_handler['Q'] = &PgClient::handleQuery;

	m_handler['P'] = &PgClient::handleParse;
	m_handler['B'] = &PgClient::handleBind;
	m_handler['D'] = &PgClient::handleDescribe;
	m_handler['E'] = &PgClient::handleExecute;
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
	m_pWorker->parse(sql);

	m_pPlan = m_pWorker->resolve();

	describeColumn();
	handleExecute();
}

void PgClient::handleParse() {
	auto sStmt = m_receiver.getNextString(); //statement name
	auto sql = m_receiver.getNextString();

	DLOG(INFO)<< "STMT:" <<sStmt<<", SQL:"<< sql;

	m_pWorker->parse(sql);

	size_t iParamNum = m_receiver.getNextShort();
	size_t iActualNum =m_pWorker->getBindParamNumber();

	if (iParamNum != iActualNum) {
		throw new ParseException(ConcateToString(
				"Parameter number unmatch!, expect ", iParamNum, ", actual ", iActualNum));
	}

	m_sender.prepare('1');
	m_sender.commit();
}

void PgClient::handleBind() {
	auto portal = m_receiver.getNextString();
	auto stmt = m_receiver.getNextString();

	DLOG(INFO)<< "B:portal "<<portal <<" ,stmt "<<stmt;

	size_t iActualNum =m_pWorker->getBindParamNumber();

	size_t iExpectNum = m_receiver.getNextShort();
	if (iExpectNum != iActualNum) {
		PARSE_ERROR(ConcateToString(
				"Parameter format number unmatch!, expect ", iExpectNum, ", actual ", iActualNum));
	}
	std::vector<Operation> types(iActualNum);
	for (size_t i = 0; i < iActualNum; ++i) {
		auto type = m_receiver.getNextShort();
		DLOG(INFO)<< "Bind type:"<<type;
		switch(type) {
		case PARAM_TEXT_MODE:
			types[i] = Operation::TEXT_PARAM;
			break;
		case PARAM_BINARY_MODE:
			types[i] = Operation::BINARY_PARAM;
			break;
		default:
			IO_ERROR("Unexpected bind parameter mode: ", type);
			break;
		}
	}


	iExpectNum = m_receiver.getNextShort();
	if (iExpectNum != iActualNum) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iExpectNum , ", actual " , iActualNum);
	}

	m_pWorker->markParseBuffer();

	for (int i = 0; i < iActualNum; ++i) {
		auto pParam = m_pWorker->getBindParam(i);
		pParam->setBindParamMode(types[i]);
		auto s = m_receiver.getNextStringWithLen();
		pParam->setString(m_pWorker->allocString(s, true));
	}

	m_sender.prepare('2');
	m_sender.commit();

	m_pPlan = m_pWorker->resolve();
}

void PgClient::handleDescribe() {
	int type = m_receiver.getNextByte();
	auto sName = m_receiver.getNextString();

	DLOG(INFO)<< "D:type "<<type << ",name "<< sName;

	describeColumn();

}

void PgClient::handleExecute() {
	if (m_pPlan == nullptr) {
		PARSE_ERROR("No statement!");
	}
	size_t columnNum = m_pPlan->getResultColumns();

	m_pPlan->begin();

	while (m_pPlan->next()) {
		if (columnNum == 0)
			continue;
		sendRow();
	} //while
	m_sender.flush();

	auto sInfo = m_pPlan->getInfoString();

	m_pPlan->end();
	DLOG(INFO)<< "Execute result:" << sInfo;

	m_sender.prepare('C');
	m_sender.addStringZeroEnd(sInfo); //data len
	m_sender.commit();

	m_pPlan = nullptr;

	//discard allocated string for bind param
	m_pWorker->restoreParseBuffer();
}

void PgClient::handleException(Exception* pe) {
	m_sender.prepare('E');
	m_sender.addByte(PG_DIAG_SEVERITY);
	m_sender.addStringZeroEnd("ERROR");
	m_sender.addByte(PG_DIAG_SQLSTATE);
	m_sender.addStringZeroEnd("00000");
	m_sender.addByte(PG_DIAG_MESSAGE_PRIMARY);
	m_sender.addStringZeroEnd(pe->what());

	if (pe->getLine() >= 0) {
		m_sender.addByte(PG_DIAG_STATEMENT_POSITION);
		m_sender.addStringZeroEnd(std::to_string(pe->getStartPos()));
	}
	m_sender.addByte('\0');
	delete pe;

	m_sender.commit();

	m_pPlan = nullptr;
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
	m_sender.addStringZeroEnd("server_encoding");
	m_sender.addStringZeroEnd("UTF8");
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addStringZeroEnd("client_encoding");
	m_sender.addStringZeroEnd("UTF8");
	m_sender.commit();

	m_sender.prepare('S');
	m_sender.addStringZeroEnd("server_version");
	m_sender.addStringZeroEnd("9.0.4");
	m_sender.commit();

	m_sender.prepare('K');
	m_sender.addInt(m_pWorker->getIndex());
	m_sender.addInt(0); //cancel key
	m_sender.commit();

	handleSync();
#ifndef NO_TIMEING
	auto end = std::chrono::steady_clock::now();

	m_pWorker->m_iClientTime += std::chrono::duration_cast
			< std::chrono::microseconds > (end - start).count();
#endif

	while (!m_bGlobalTerminate.load()) {
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
			IO_ERROR("Unable to handler message ", qtype);
		}

		try {
			(this->*handler)();
		} catch (Exception* pe) {
			handleException(pe);
		}
		if (qtype == 'Q')
			handleSync();

#ifndef NO_TIMEING
		auto end = std::chrono::steady_clock::now();

		m_pWorker->m_iClientTime += std::chrono::duration_cast
				< std::chrono::microseconds > (end - start).count();
#endif
	} //while
}


void PgClient::describeColumn() {

	if (m_pPlan == nullptr) {
		PARSE_ERROR("No statement!");
	}

	size_t columnNum = m_pPlan->getResultColumns();
	if (columnNum == 0) {
		DLOG(INFO) << "No columns";
		return;
	}

	m_sender.prepare('T');
	m_sender.addShort(columnNum);

	for (size_t i = 0; i < columnNum; ++i) {
		auto sName = m_pPlan->getProjectionName(i);

		switch (m_pPlan->getResultType(i)) {
		case DBDataType::BYTES:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Bytea, -1, false);
			break;
		case DBDataType::INT8:
		case DBDataType::INT16:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int16, 2, false);
			break;
		case DBDataType::INT32:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int32, 4, false);
			break;
		case DBDataType::INT64:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Int64, 8, false);
			break;
		case DBDataType::STRING:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Varchar, -1, false);
			break;
		case DBDataType::DATETIME:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::DATE:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::FLOAT:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Float, -1, false);
			break;
		case DBDataType::DOUBLE:
			m_sender.addDataTypeMsg(sName, i + 1, PgDataType::Double, -1, false);
			break;
		default:
			LOG(ERROR) << "Unknown type for " << sName;
			assert(0);
			break;
		}
	}
	m_sender.commit();
	m_sender.flush();
}

void PgClient::sendRow() {
	size_t columnNum = m_pPlan->getResultColumns();
	assert(columnNum > 0);
	m_sender.prepare('D');
	m_sender.addShort(columnNum); //field number
	for (size_t i = 0; i < columnNum; ++i) {
		try {
			DBDataType type = m_pPlan->getResultType(i);

			ExecutionResult result;

			m_pPlan->getResult(i, result);


			if (result.isNull()) {
				m_sender.addInt(-1);
				continue;
			}

			switch (type) {
			case DBDataType::INT8:
			case DBDataType::INT32:
			case DBDataType::INT64:
			case DBDataType::INT16:
				m_sender.addIntAsString(result.getInt());
				break;
			case DBDataType::BYTES: {
				std::ostringstream os;
				os << "0x" ;
				for (auto& c: result.getString()) {
					os<< std::hex<< (int)c ;
				}
				m_sender.addString(os.str());
				break;
			}
			case DBDataType::STRING:
				m_sender.addString(result.getString());
				break;
			case DBDataType::DATE: {
				time_t time = result.getInt();
				struct tm* pToday = gmtime(&time);
				if (pToday == nullptr) {
					LOG(ERROR) << "Failed to get localtime "<< (int ) time;
					m_sender.addInt(0);
				} else {
					m_sender.addDateAsString(pToday);
				}
				break;
			}
			case DBDataType::DATETIME: {
				time_t time = result.getInt();
				struct tm* pToday = gmtime(&time);
				if (pToday == nullptr) {
					LOG(ERROR) << "Failed to get gmtime "<< (int ) time;
					m_sender.addInt(0);
				} else {
					m_sender.addDateTimeAsString(pToday);
				}
				break;
			}
			case DBDataType::FLOAT:
			case DBDataType::DOUBLE: {
				m_sender.addDoubleAsString(result.getDouble());
				break;
			}
			default:
				assert(0);
				break;
			}; //switch
		} catch (...) {
			for (; i < columnNum; ++i)
				m_sender.addInt(-1);
			m_sender.commit();
			m_sender.flush();
			throw;
		}
	} //for

	m_sender.commit();
}
