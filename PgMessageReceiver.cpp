#include "PgMessageReceiver.h"
#include "IOException.h"
#include <stdio.h>
#include <cassert>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <glog/logging.h>
#include "execution/ExecutionPlan.h"
#include "execution/WorkThreadInfo.h"

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

constexpr ProtocolVersion CANCEL_REQUEST_CODE = PG_PROTOCOL(1234, 5678);
constexpr ProtocolVersion NEGOTIATE_SSL_CODE = PG_PROTOCOL(1234, 5679);

}
PgMessageReceiver::PgMessageReceiver(int fd) :
		DataReceiver(fd) {
}

PgMessageReceiver::~PgMessageReceiver() {
}

void PgMessageReceiver::processStartupPacket() {
	readData();

	size_t iLen = getDataLen();
	if (iLen < sizeof(ProtocolVersion) || iLen > MAX_STARTUP_PACKET_LENGTH) {
		IO_ERROR("Illegal startup packet length!");
	}

	ProtocolVersion proto = (ProtocolVersion) getNextInt();
	if (proto == CANCEL_REQUEST_CODE) {
		uint32_t iBackendPID = getNextInt();
		uint32_t iCancelAuthCode = getNextInt();
		WorkerManager::getInstance().getWorker(
				iBackendPID)->cancel();
		IO_ERROR("Cacnel WorkerID=", iBackendPID,", CancelAuthCode=",	iCancelAuthCode);
	}
	if (proto == NEGOTIATE_SSL_CODE) {
		if (write(getFd(), "N", 1) != 1) // SSL is not supported
				{
			IO_ERROR("send() failed!");
		}
		processStartupPacket();
		return;
	}
	if (PG_PROTOCOL_MAJOR(proto) < 3) {
		IO_ERROR("Unsupported protocol!");
	}
	while (hasData()) {
		size_t iLen;
		auto sName = getNextString();
		if (sName == "")
			break;
		if (!hasData())
			break;

		DLOG(INFO)<< "option:" << sName <<"="<< getNextString();
	}

}

char PgMessageReceiver::readMessage() {
	char qtype = readByte();
	if (qtype == EOF || qtype == 'X') {
		return 'X';
	}
	readData();

	return qtype;
}
