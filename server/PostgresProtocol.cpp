#include "common/MetaConfig.h"
#include "execution/ExecutionPlan.h"

#include "WorkThreadInfo.h"
#include "IOException.h"
#include "PostgresProtocol.h"
#include "WorkerManager.h"

namespace {

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
: m_iSessionIndex(iSessionIndex)
, m_buffer(MetaConfig::getInstance().getNetworkBuffer(), '\0')
, m_sender(m_buffer){

}

void PostgresProtocol::startup(int fd) {
	PgDataReader receiver(readData(fd));

	ProtocolVersion proto = (ProtocolVersion) receiver.getNextInt();
	if (proto == CANCEL_REQUEST_CODE) {
		uint32_t iBackendPID = receiver.getNextInt();
		uint32_t iCancelAuthCode = receiver.getNextInt();

		auto pWorker = WorkerManager::getInstance().getWorker(iBackendPID);
		assert(pWorker);
		pWorker->cancel(true);

		IO_ERROR("Cacnel WorkerID=", iBackendPID, ", CancelAuthCode=", iCancelAuthCode);
	}
	if (proto == NEGOTIATE_SSL_CODE) {
		if(::write(fd, "N", 1) != 1 ){
			IO_ERROR("Send N for SSL failed");
		}
		startup(fd);
		return;
	}
	if (PG_PROTOCOL_MAJOR(proto) < 3) {
		IO_ERROR("Unsupported protocol!");
	}
	while (receiver.hasData()) {
		size_t iLen;
		auto sName = receiver.getNextString();
		if (sName == "")
			break;
		auto sValue = receiver.getNextString();

		LOG(INFO)<< "option:" << sName <<"="<< sValue;
	}

	sendShortMessage('R', AUTH_REQ_OK);
	sendShortMessage('S', "server_encoding", "UTF8");
	sendShortMessage('S', "client_encoding", "UTF8");
	sendShortMessage('S', "server_version", "9.0.4");
	sendShortMessage('K', m_iSessionIndex, (int32_t)0);
	sendSync(fd);
}
char PostgresProtocol::readMessageType(int fd) {
	char qtype;
	if(int ret = ::recv(fd, &qtype, 1, 0); ret != 1) {
		IO_ERROR("read() failed!");
	}
	if (qtype == EOF) {
		return 'X';
	}
	return qtype;
}

std::string_view PostgresProtocol::readData(int fd) {
	size_t iLen = 0;
	if (::recv(fd, (char*) &iLen, 4, 0) != 4) {
		IO_ERROR("Unexpect EOF!");
	}

	iLen = ntohl(iLen);
	if (iLen < 4) {
		IO_ERROR("Invalid message length!");
	}

	iLen -= 4;

	assert(m_sender.empty());

	if(m_buffer.size() < iLen + 1) {
		IO_ERROR("Not enough receive buffer");
	}
	size_t readCount = 0;
	while (iLen > readCount) {
		int count = ::read(fd, m_buffer.data() + readCount, iLen - readCount);
		if (count < 0) {
			IO_ERROR("read() failed!");
		}
		readCount += count;
	}

	return std::string_view(m_buffer.data(), iLen);
}

void PostgresProtocol::readBindParam(size_t iParamNum, PgDataReader& receiver, std::function<ReadParamFn> readParamFn) {
	assert(m_sender.empty());

	auto portal = receiver.getNextString();
	auto stmt = receiver.getNextString();

	DLOG(INFO)<< "B:portal "<<portal <<" ,stmt "<<stmt;

	size_t iExpectNum = receiver.getNextShort();
	if (iExpectNum != iParamNum) {
		PARSE_ERROR("Parameter format number unmatch!, expect ", iExpectNum, ", actual ", iParamNum);
	}
	std::vector<bool> isBinary(iParamNum);
	for (size_t i = 0; i < iParamNum; ++i) {
		auto type = receiver.getNextShort();
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


	iExpectNum = receiver.getNextShort();
	if (iExpectNum != iParamNum) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iExpectNum , ", actual " , iParamNum);
	}

	for (int i = 0; i < iParamNum; ++i) {
		readParamFn(i,receiver.getNextStringWithLen(), isBinary[i]);
	}
}

void PostgresProtocol::flushSend(int fd) {
	size_t iLastPrepared = m_sender.getLastPrepared();
	if(iLastPrepared == 0) {
		return;
	}
	//Because we must write back package length at m_iLastPrepare.
	//We could not send data after m_iLastPrepare.
	uint32_t nWrite = ::send(fd, m_buffer.data(), iLastPrepared, 0);
	if (nWrite != iLastPrepared) {
		IO_ERROR("Could not send data\n");
	}
	m_sender.clear();
}
