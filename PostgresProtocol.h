#pragma once

#include "DataSender.h"
#include "DataReceiver.h"
#include "MessageSender.h"


class PostgresProtocol {
public:
	PostgresProtocol(int fd, int32_t iSessionIndex);
	void startup();
	char readMessage();

	template<typename ...Args>
	void sendShortMessage(char cMsgType, Args ...args) {
		MessageSender sender(m_sender, cMsgType);
		(m_sender << ... << args);
	}

	void sendShortMessage(char cMsgType) {
		MessageSender sender(m_sender, cMsgType);
	}

	void sendData(ExecutionPlan* pPlan, bool binaryMode) {
		MessageSender sender(m_sender, 'D');
		if(binaryMode) {
			BinaryModeDataSender dataSender(m_sender);
			sender.sendData(pPlan, dataSender);
		} else {
			TextModeDataSender dataSender(m_sender);
			sender.sendData(pPlan, dataSender);
		}
	}

	void sendColumnDescription(ExecutionPlan* pPlan) {
		size_t columnNum;
		if (pPlan == nullptr) {
			columnNum = 0;
		} else {
			columnNum = pPlan->getResultColumns();
		}
	    if (columnNum == 0) {
			DLOG(INFO) << "No columns";
			return;
		}
		MessageSender sender(m_sender, 'T');

		sender.sendColumnDescription(pPlan, columnNum);
		m_sender.flush();
	}

	void sendException(Exception* pe) {
		MessageSender sender(m_sender, 'E');
		sender.sendException(pe);
	}

	std::string_view readQueryInfo() {
		auto sql = m_receiver.getNextString();
		DLOG(INFO) << "Q:"<< sql;
		return sql;
	}

	std::pair<std::string_view, size_t> readParseInfo() {
		auto sStmt = m_receiver.getNextString(); //statement name
		auto sql = m_receiver.getNextString();

		DLOG(INFO)<< "STMT:" <<sStmt<<", SQL:"<< sql;

		size_t iParamNum = m_receiver.getNextShort();

		return std::make_pair(sql, iParamNum);
	}

	void readColumnDescribeInfo() {
		int type = m_receiver.getNextByte();
		auto sName = m_receiver.getNextString();

		DLOG(INFO)<< "D:type "<<type << ",name "<< sName;
	}

	void flush() {
		m_sender.flush();
	}

	void sendSync() {
		DLOG(INFO) << "sync";
		sendShortMessage('Z', static_cast<int8_t>('I'));
		m_sender.flush();
	}

	using ReadParamFn = void (size_t index, std::string_view value, bool isBinary);
	void readBindParam(size_t iParamNum,
			std::function<ReadParamFn> readParamFn);
private:

	DataSender m_sender;
	DataReceiver m_receiver;
	int32_t m_iSessionIndex;
};
