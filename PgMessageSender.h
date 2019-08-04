#ifndef PG_MESSAGE_SENDER_H
#define PG_MESSAGE_SENDER_H

#include "common/DataSender.h"

class PgMessageSender: public DataSender
{
public:
	PgMessageSender(int fd);
	virtual ~PgMessageSender();
	enum PgDataType
	{
		Bytea = 17,
		Int16 = 21,
		Int32 = 23,
		Int64 = 20,
		Varchar = 1043,
		DateTime = 1114,
		Double = 701,
	};

	void prepare(char cMsgType);
	void commit();

	void addDataTypeMsg(const char* pszName, short columnid, PgDataType type,
			short datalen)
	{
		addString(pszName, strlen(pszName) + 1);
		addInt(0); //oid
		addShort(columnid);
		addInt(type);
		addShort(datalen);
		addInt(-1); //typemod
		addShort(0); //format
	}
};
#endif
