#pragma once

#include "DataSender.h"
#include "common/ParseNode.h"

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;

//https://jdbc.postgresql.org/development/privateapi/constant-values.html
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
			short datalen, bool binary ) {
		addStringZeroEnd(sName);
		addInt(0); //oid
		addShort(columnid);
		addInt(static_cast<int32_t>(type));
		addShort(datalen);
		addInt(-1); //typemod
		addShort(binary ? PARAM_BINARY_MODE : PARAM_TEXT_MODE); //format
	}
};
