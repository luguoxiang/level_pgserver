#pragma once

#include "PgDataWriter.h"
#include "PgDataReader.h"
#include "PgMessageWriter.h"


class PostgresProtocol {
public:
	PostgresProtocol(int32_t iSessionIndex);
	void startup(int fd);

	char readMessageType(int fd);
	std::string_view readData(int fd);

	template<typename ...Args>
	void sendShortMessage(char cMsgType, Args ...args) {
		PgMessageWriter sender(m_sender, cMsgType);
		(m_sender << ... << args);
		if(m_sender.isBufferFull()) {
			IO_ERROR("not enough send buffer");
		}
	}

	template<typename ...Args>
	void sendShortMessage(int fd, char cMsgType, Args ...args) {
		PgMessageWriter sender(m_sender, cMsgType);
		(m_sender << ... << args);
		if(m_sender.isBufferFull()) {
			flushSend(fd);
			sendShortMessage(cMsgType, args...);
		}
	}

	void sendShortMessage(char cMsgType) {
		PgMessageWriter sender(m_sender, cMsgType);
		if(m_sender.isBufferFull()) {
			IO_ERROR("not enough send buffer");
		}
	}

	void sendShortMessage(int fd, char cMsgType) {
		PgMessageWriter sender(m_sender, cMsgType);
		if(m_sender.isBufferFull()) {
			flushSend(fd);
			sendShortMessage(cMsgType);
		}
	}

	bool sendData(ExecutionPlan* pPlan) {
		PgMessageWriter sender(m_sender, 'D');
		sender.sendData(pPlan);
		return !m_sender.isBufferFull();
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
		PgMessageWriter sender(m_sender, 'T');

		sender.sendColumnDescription(pPlan, columnNum);

		if(m_sender.isBufferFull()) {
			IO_ERROR("not enough send buffer");
		}
	}


	void sendException(std::exception& e, int startPos) {
		PgMessageWriter sender(m_sender, 'E');
		sender.sendException(e, startPos);
		if(m_sender.isBufferFull()) {
			IO_ERROR("not enough send buffer");
		}
	}

	std::string_view readQueryInfo(PgDataReader& receiver) {
		assert(m_sender.empty());

		auto sql = receiver.getNextString();
		DLOG(INFO) << "Q:"<< sql;
		return sql;
	}

	std::pair<std::string_view, size_t> readParseInfo(PgDataReader& receiver) {
		assert(m_sender.empty());

		auto sStmt = receiver.getNextString(); //statement name
		auto sql = receiver.getNextString();

		DLOG(INFO)<< "STMT:" <<sStmt<<", SQL:"<< sql;

		size_t iParamNum = receiver.getNextShort();

		return std::make_pair(sql, iParamNum);
	}

	void readColumnDescribeInfo(PgDataReader& receiver) {
		assert(m_sender.empty());

		int type = receiver.getNextByte();
		auto sName = receiver.getNextString();

		DLOG(INFO)<< "D:type "<<type << ",name "<< sName;
	}

	void flushSend(int fd);

	void clear() {
		m_sender.clear();
	}

	void sendSync(int fd) {
		DLOG(INFO) << "sync";
		sendShortMessage('Z', static_cast<int8_t>('I'));
		flushSend(fd);
	}

	using ReadParamFn = void (size_t index, std::string_view value, bool isBinary);
	void readBindParam(size_t iParamNum, PgDataReader& receiver,
			std::function<ReadParamFn> readParamFn);
private:

	PgDataWriter m_sender;
	int32_t m_iSessionIndex;
	std::string m_buffer;
};
