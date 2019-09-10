#pragma once

#include "RowDataSender.h"
#include "execution/ExecutionPlan.h"

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;

class MessageSender {
public:
	MessageSender(DataSender& sender, char cMsgType) :m_sender(sender) {
		m_sender.begin();
		m_sender.addByte(cMsgType);
		m_sender.addInt(0); //write back later
	};

	void sendData(ExecutionPlan* pPlan, RowDataSender& dataSender);

	void sendColumnDescription(ExecutionPlan* pPlan, size_t columnNum);

	void sendException(Exception* pe);

	~MessageSender() {
		m_sender.setInt(1, m_sender.getWritten() - 1);
		m_sender.end();
	}
private:
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

	void addDataTypeMsg(std::string_view sName,
			int16_t columnid,
			PgDataType type,
			int16_t datalen,
			bool binary) {
		m_sender<< sName
				<< static_cast<int32_t>(0) //oid
				<< columnid
				<<static_cast<int32_t>(type)
				<< datalen
				<< static_cast<int32_t>(-1) //typemod
				<< (binary ? PARAM_BINARY_MODE : PARAM_TEXT_MODE);
	}

	DataSender& m_sender;
};
