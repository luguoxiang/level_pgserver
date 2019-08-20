#pragma once

#include "DataSender.h"

enum class PgDataType {
	Bytea = 17,
	Int16 = 21,
	Int32 = 23,
	Int64 = 20,
	Varchar = 1043,
	DateTime = 1114,
	Float = 700,
	Double = 701,
};
class PgMessageSender: public DataSender {
public:
	PgMessageSender(int fd);
	virtual ~PgMessageSender();

	void prepare(char cMsgType);
	void commit();

	void addDataTypeMsg(const std::string_view sName, short columnid, PgDataType type,
			short datalen) {
		addString(sName);
		addInt(0); //oid
		addShort(columnid);
		addInt(static_cast<int32_t>(type));
		addShort(datalen);
		addInt(-1); //typemod
		addShort(0); //format
	}
};
