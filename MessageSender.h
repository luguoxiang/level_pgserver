#pragma once

#include <map>
#include "DataSender.h"
#include "execution/ExecutionPlan.h"

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;

class MessageSender {
public:
	MessageSender(DataSender& sender, char cMsgType) :m_sender(sender) {
		m_sender.begin();
		m_sender<< static_cast<int8_t>(cMsgType);
		m_sender<< static_cast<int32_t>(0); //write back later
	};

	void sendData(ExecutionPlan* pPlan);

	void sendColumnDescription(ExecutionPlan* pPlan, size_t columnNum);

	void sendException(Exception* pe);

	~MessageSender() {
		m_sender.setInt(1, m_sender.getWritten() - 1);
		m_sender.end();
	}
	static void init();
private:
	//https://jdbc.postgresql.org/development/privateapi/constant-values.html
	enum class PgDataType {
		Bool = 16,
		Bytea = 17,
		Int16 = 21,
		Int32 = 23,
		Int64 = 20,
		Varchar = 1043,
		Date = 1082,
		DateTime = 1114,
		Float = 700,
		Double = 701,
	};

	void addDataTypeMsg(std::string_view sName,
			int16_t columnid,
			PgDataType type,
			int16_t datalen) {
		m_sender<< sName
				<< static_cast<int32_t>(0) //oid
				<< columnid
				<<static_cast<int32_t>(type)
				<< datalen
				<< static_cast<int32_t>(-1) //typemod
				<< PARAM_TEXT_MODE;
	}


	using SendFn = std::function<void (ExecutionResult& result, DataSender& sender)>;
	static std::map<DBDataType, std::pair<PgDataType,SendFn>> m_typeHandler;

	DataSender& m_sender;
};
