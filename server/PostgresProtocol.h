#pragma once

#include "PgDataWriter.h"
#include "PgDataReader.h"

class ExecutionResult;
class ExecutionPlan;

class PostgresProtocol {
public:
	enum class StartupAction {
		SSL, Normal
	};

	static void init();

	StartupAction readStartup(MemBuffer* pBuffer, size_t len);

	std::string_view readQueryInfo(MemBuffer* pBuffer, size_t len) {
		PgDataReader receiver(pBuffer, len);
		auto sql = receiver.getNextString();
		DLOG(INFO)<< "Q:"<< sql;
		return sql;
	}

	std::pair<std::string_view, size_t> readParseInfo(MemBuffer* pBuffer, size_t len) {
		PgDataReader receiver(pBuffer, len);
		auto sStmt = receiver.getNextString(); //statement name
		auto sql = receiver.getNextString();

		DLOG(INFO)<< "STMT:" <<sStmt<<", SQL:"<< sql;

		size_t iParamNum = receiver.getNextShort();

		return std::make_pair(sql, iParamNum);
	}

	void readColumnDescribeInfo(MemBuffer* pBuffer, size_t len) {
		PgDataReader receiver(pBuffer, len);

		int type = receiver.getNextByte();
		auto sName = receiver.getNextString();

		DLOG(INFO)<< "D:type "<<type << ",name "<< sName;
	}

	using ParamInfo =std::pair<bool, std::string_view>;
	void readBindParam(MemBuffer* pBuffer, size_t len, std::vector<ParamInfo>& params);

	size_t buildStartupResponse(MemBuffer* pBuffer, int32_t iSessionIndex) noexcept;

	size_t buildSync(MemBuffer* pBuffer) noexcept {
		PgDataWriter writer(pBuffer);
		writeShortMessage(writer, 'Z', static_cast<int8_t>('I'));
		assert(!writer.isBufferFull());

		return writer.getLastPrepared();
	}

	size_t buildColumnDescription(MemBuffer* pBuffer, ExecutionPlan* pPlan, size_t columnNum);

	void buildData(PgDataWriter& writer, ExecutionPlan* pPlan, size_t columnNum);

	size_t buildException(MemBuffer* pBuffer, const std::string& msg, int startPos, bool sync) noexcept;

	size_t buildExecutionDone(MemBuffer* pBuffer, bool parsed, bool bind, std::string_view sInfo, bool sync) noexcept;

private:
	template<typename ...Args>
	void writeShortMessage(PgDataWriter& writer, char cMsgType, Args ...args) {
		writer.begin(cMsgType);
		(writer << ... << args);
		writer.end();
	}
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
	using WriteFn = std::function<void (ExecutionResult& , PgDataWriter& )>;
	static std::map<DBDataType, std::pair<PgDataType,WriteFn>> m_typeHandler;

};
