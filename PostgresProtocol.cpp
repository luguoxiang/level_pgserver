#include "common/MetaConfig.h"
#include "execution/ExecutionPlan.h"
#include "execution/WorkThreadInfo.h"
#include "IOException.h"
#include "PostgresProtocol.h"

namespace {
constexpr size_t MAX_STARTUP_PACKET_LENGTH = 10000;

/* FE/BE protocol version number */
using ProtocolVersion=unsigned int;
using PacketLen=unsigned int;

constexpr ProtocolVersion PG_PROTOCOL(int m, int n) {
	return (((m) << 16) | (n));
}
constexpr ProtocolVersion PG_PROTOCOL_MAJOR(int v) {
	return ((v) >> 16);
}
constexpr ProtocolVersion PG_PROTOCOL_MINOR(int v) {
	return ((v) & 0x0000ffff);
}
//https://www.postgresql.org/docs/current/protocol-message-formats.html
//CancelRequest
constexpr ProtocolVersion CANCEL_REQUEST_CODE = PG_PROTOCOL(1234, 5678);
//SSLRequest
constexpr ProtocolVersion NEGOTIATE_SSL_CODE = PG_PROTOCOL(1234, 5679);

constexpr int32_t AUTH_REQ_OK = 0; /* User is authenticated  */
constexpr int32_t AUTH_REQ_PASSWORD = 3; /* Password */

constexpr int8_t PG_DIAG_SEVERITY = 'S';
constexpr int8_t PG_DIAG_SQLSTATE = 'C';
constexpr int8_t PG_DIAG_MESSAGE_PRIMARY = 'M';
constexpr int8_t PG_DIAG_MESSAGE_DETAIL = 'D';
constexpr int8_t PG_DIAG_MESSAGE_HINT = 'H';
constexpr int8_t PG_DIAG_STATEMENT_POSITION = 'P';
constexpr int8_t PG_DIAG_INTERNAL_POSITION = 'p';
constexpr int8_t PG_DIAG_INTERNAL_QUERY = 'q';
constexpr int8_t PG_DIAG_CONTEXT = 'W';
constexpr int8_t PG_DIAG_SOURCE_FILE = 'F';
constexpr int8_t PG_DIAG_SOURCE_LINE = 'L';
constexpr int8_t PG_DIAG_SOURCE_FUNCTION = 'R';

}

void MessageSender::sendException(Exception* pe) {
	std::string msg = pe->what();

	m_sender<< PG_DIAG_SEVERITY << "ERROR"
			<< PG_DIAG_SQLSTATE << "00000"
			<< PG_DIAG_MESSAGE_PRIMARY << msg;

	if (pe->getLine() >= 0) {
		std::string pos = std::to_string(pe->getStartPos());
		m_sender << PG_DIAG_STATEMENT_POSITION << pos;
	}
	m_sender << static_cast<int8_t>(0);
}


void MessageSender::sendColumnDescription(ExecutionPlan* pPlan, size_t columnNum) {
	assert(columnNum > 0);
	m_sender.addShort(columnNum);

	for (size_t i = 0; i < columnNum; ++i) {
		auto sName = pPlan->getProjectionName(i);

		switch (pPlan->getResultType(i)) {
		case DBDataType::BYTES:
			addDataTypeMsg(sName, i + 1, PgDataType::Bytea, -1, false);
			break;
		case DBDataType::INT8:
		case DBDataType::INT16:
			addDataTypeMsg(sName, i + 1, PgDataType::Int16, 2, false);
			break;
		case DBDataType::INT32:
			addDataTypeMsg(sName, i + 1, PgDataType::Int32, 4, false);
			break;
		case DBDataType::INT64:
			addDataTypeMsg(sName, i + 1, PgDataType::Int64, 8, false);
			break;
		case DBDataType::STRING:
			addDataTypeMsg(sName, i + 1, PgDataType::Varchar, -1, false);
			break;
		case DBDataType::DATETIME:
			addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::DATE:
			addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::FLOAT:
			addDataTypeMsg(sName, i + 1, PgDataType::Float, -1, false);
			break;
		case DBDataType::DOUBLE:
			addDataTypeMsg(sName, i + 1, PgDataType::Double, -1, false);
			break;
		default:
			LOG(ERROR) << "Unknown type for " << sName;
			assert(0);
			break;
		}
	}
}

void MessageSender::sendData(ExecutionPlan* pPlan, bool binaryMode) {
	size_t columnNum;
	if (pPlan == nullptr) {
		columnNum = 0;
	} else {
		columnNum = pPlan->getResultColumns();
	}

	m_sender.addShort(columnNum);
	for (size_t i = 0; i < columnNum; ++i) {
		try {
			DBDataType type = pPlan->getResultType(i);

			ExecutionResult result;

			pPlan->getResult(i, result, type);


			if (result.isNull()) {
				m_sender.addInt(-1);
				continue;
			}
			switch (type) {
			case DBDataType::BYTES: {
				m_sender.addBytesString(result.getString());
				continue;
			}
			case DBDataType::STRING:
				m_sender.addString(result.getString());
				continue;
			default:
				break;
			}
			if(!binaryMode) {
				switch (type) {
				case DBDataType::INT8:
				case DBDataType::INT32:
				case DBDataType::INT64:
				case DBDataType::INT16:
					m_sender.addIntAsString(result.getInt());
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
			} else {
				switch (type) {
				case DBDataType::INT32:
					m_sender.addInt(4);
					m_sender.addInt(result.getInt());
					break;
				case DBDataType::INT64:
					m_sender.addInt(8);
					m_sender.addInt64(result.getInt());
					break;
				case DBDataType::INT8:
				case DBDataType::INT16:
					m_sender.addInt(2);
					m_sender.addShort(result.getInt());
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
					m_sender.addInt(4);
					m_sender.addFloat(result.getDouble());
					break;
				case DBDataType::DOUBLE: {
					m_sender.addInt(8);
					m_sender.addDouble(result.getDouble());
					break;
				}
				default:
					assert(0);
					break;
				};
			}
		} catch (...) {
			for (; i < columnNum; ++i)
				m_sender.addInt(-1);
			throw;
		}
	} //for
}


PostgresProtocol::PostgresProtocol(int fd, int32_t iSessionIndex)
: m_sender(fd, MetaConfig::getInstance().getNetworkBuffer())
, m_receiver(fd)
, m_iSessionIndex(iSessionIndex) {

}

void PostgresProtocol::startup() {
	m_receiver.readData();

	size_t iLen = m_receiver.getDataLen();
	if (iLen < sizeof(ProtocolVersion) || iLen > MAX_STARTUP_PACKET_LENGTH) {
		IO_ERROR("Illegal startup packet length!");
	}

	ProtocolVersion proto = (ProtocolVersion) m_receiver.getNextInt();
	if (proto == CANCEL_REQUEST_CODE) {
		uint32_t iBackendPID = m_receiver.getNextInt();
		uint32_t iCancelAuthCode = m_receiver.getNextInt();

		auto pWorker = WorkerManager::getInstance().getWorker(iBackendPID);
		assert(pWorker);
		pWorker->cancel(true);

		IO_ERROR("Cacnel WorkerID=", iBackendPID, ", CancelAuthCode=", iCancelAuthCode);
	}
	if (proto == NEGOTIATE_SSL_CODE) {
		m_sender.directSend("N");
		startup();
		return;
	}
	if (PG_PROTOCOL_MAJOR(proto) < 3) {
		IO_ERROR("Unsupported protocol!");
	}
	while (m_receiver.hasData()) {
		size_t iLen;
		auto sName = m_receiver.getNextString();
		if (sName == "")
			break;
		auto sValue = m_receiver.getNextString();

		LOG(INFO)<< "option:" << sName <<"="<< sValue;
	}

	sendShortMessage('R', AUTH_REQ_OK);
	sendShortMessage('S', "server_encoding", "UTF8");
	sendShortMessage('S', "client_encoding", "UTF8");
	sendShortMessage('S', "server_version", "9.0.4");
	sendShortMessage('K', m_iSessionIndex, (int32_t)0);
	sendSync();
}


char PostgresProtocol::readMessage() {
	char qtype = m_receiver.readByte();
	if (qtype == EOF || qtype == 'X') {
		return 'X';
	}
	m_receiver.readData();

	return qtype;
}

void PostgresProtocol::readBindParam(size_t iParamNum, std::function<ReadParamFn> readParamFn) {
	auto portal = m_receiver.getNextString();
	auto stmt = m_receiver.getNextString();

	DLOG(INFO)<< "B:portal "<<portal <<" ,stmt "<<stmt;

	size_t iExpectNum = m_receiver.getNextShort();
	if (iExpectNum != iParamNum) {
		PARSE_ERROR("Parameter format number unmatch!, expect ", iExpectNum, ", actual ", iParamNum);
	}
	std::vector<bool> isBinary(iParamNum);
	for (size_t i = 0; i < iParamNum; ++i) {
		auto type = m_receiver.getNextShort();
		DLOG(INFO)<< "Bind type:"<<type;
		switch(type) {
		case PARAM_TEXT_MODE:
			isBinary[i] = false;
			break;
		case PARAM_BINARY_MODE:
			isBinary[i] = true;
			break;
		default:
			IO_ERROR("Unexpected bind parameter mode: ", type);
			break;
		}
	}


	iExpectNum = m_receiver.getNextShort();
	if (iExpectNum != iParamNum) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iExpectNum , ", actual " , iParamNum);
	}

	for (int i = 0; i < iParamNum; ++i) {
		readParamFn(i,m_receiver.getNextStringWithLen(), isBinary[i]);
	}
}
