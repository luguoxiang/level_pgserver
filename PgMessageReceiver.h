#pragma once
#include <stdio.h>
#include <stdint.h>
#include "DataReceiver.h"

class PgMessageReceiver: public DataReceiver {
public:
	PgMessageReceiver(int fd);
	~PgMessageReceiver();

	void processStartupPacket();
	char readMessage();
};
