#include "common/MetaConfig.h"
#include "execution/ExecutionPlan.h"

#include "WorkThreadInfo.h"
#include "IOException.h"
#include "PostgresProtocol.h"
#include "WorkerManager.h"

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


}

PostgresProtocol::PostgresProtocol(int32_t iSessionIndex)
: m_sender()
, m_receiver()
, m_iSessionIndex(iSessionIndex) {

}

void PostgresProtocol::startup(int fd) {
	m_receiver.readData(fd);

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
		if(!m_sender.directSend(fd, "N") ){
			IO_ERROR("Send N for SSL failed");
		}
		startup(fd);
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
	sendSync(fd);
}


char PostgresProtocol::readMessage(int fd) {
	char qtype = m_receiver.readByte(fd);
	if (qtype == EOF || qtype == 'X') {
		return 'X';
	}
	m_receiver.readData(fd);

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
