#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "common/MetaConfig.h"
#include "PgMessageSender.h"

PgMessageSender::PgMessageSender(int fd)
		: DataSender(fd, true, MetaConfig::getInstance().getNetworkBuffer())
{
}

PgMessageSender::~PgMessageSender()
{
}

void PgMessageSender::prepare(char cMsgType)
{
	begin();
	addByte(cMsgType);
	addInt(0);
}

void PgMessageSender::commit()
{
	setInt(1, getWritten() - 1);
	end();
}
