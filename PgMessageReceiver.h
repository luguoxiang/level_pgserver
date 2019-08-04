#ifndef PG_MESSAGE_RECEIVER_H
#define PG_MESSAGE_RECEIVER_H
#include <stdio.h>
#include <stdint.h>
#include "common/DataReceiver.h"

class PgMessageReceiver: public DataReceiver
{
public:
	PgMessageReceiver(int fd);
	~PgMessageReceiver();

	void processStartupPacket();
	char readMessage();
private:
	static const size_t MAX_STARTUP_PACKET_LENGTH = 10000;

	/* FE/BE protocol version number */
	typedef unsigned int ProtocolVersion;
	typedef unsigned int PacketLen;
};
#endif
