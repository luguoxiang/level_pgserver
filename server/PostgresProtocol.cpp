#include "common/MetaConfig.h"

#include "execution/ExecutionResult.h"
#include "execution/ExecutionPlan.h"
#include "execution/DBDataTypeHandler.h"
#include "PostgresProtocol.h"
#include "SessionManager.h"

namespace {

/* FE/BE protocol version number */
using ProtocolVersion=unsigned int;
using PacketLen=unsigned int;

constexpr ProtocolVersion PG_PROTOCOL(int m, int n) {
	return (((m) << 16) | (n));
}
constexpr ProtocolVersion PG_PROTOCOL_MAJOR(int v) {
	return ((v) >> 16);
}
constexpr ProtocolVersion PG_PROTOCOL_MINOR(int v) {
	return ((v) & 0x0000ffff);
}
//https://www.postgresql.org/docs/current/protocol-message-formats.html
//CancelRequest
constexpr ProtocolVersion CANCEL_REQUEST_CODE = PG_PROTOCOL(1234, 5678);
//SSLRequest
constexpr ProtocolVersion NEGOTIATE_SSL_CODE = PG_PROTOCOL(1234, 5679);

constexpr int32_t AUTH_REQ_OK = 0; /* User is authenticated  */
constexpr int32_t AUTH_REQ_PASSWORD = 3; /* Password */

constexpr int8_t PG_DIAG_SEVERITY = 'S';
constexpr int8_t PG_DIAG_SQLSTATE = 'C';
constexpr int8_t PG_DIAG_MESSAGE_PRIMARY = 'M';
constexpr int8_t PG_DIAG_MESSAGE_DETAIL = 'D';
constexpr int8_t PG_DIAG_MESSAGE_HINT = 'H';
constexpr int8_t PG_DIAG_STATEMENT_POSITION = 'P';
constexpr int8_t PG_DIAG_INTERNAL_POSITION = 'p';
constexpr int8_t PG_DIAG_INTERNAL_QUERY = 'q';
constexpr int8_t PG_DIAG_CONTEXT = 'W';
constexpr int8_t PG_DIAG_SOURCE_FILE = 'F';
constexpr int8_t PG_DIAG_SOURCE_LINE = 'L';
constexpr int8_t PG_DIAG_SOURCE_FUNCTION = 'R';

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;
}

decltype(PostgresProtocol::m_typeHandler) PostgresProtocol::m_typeHandler;
void PostgresProtocol::init() {
	m_typeHandler[DBDataType::BYTES] = std::make_pair(PgDataType::Bytea,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addBytesString(result.getString());
			});

	m_typeHandler[DBDataType::BOOL] = std::make_pair(PgDataType::Bool,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addString(result.getInt()?"true":"false");
			});

	m_typeHandler[DBDataType::INT16] = std::make_pair(PgDataType::Int16,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addValueAsString(result.getInt(), "%lld");
			});

	m_typeHandler[DBDataType::INT32] = std::make_pair(PgDataType::Int32,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addValueAsString(result.getInt(), "%lld");
			});

	m_typeHandler[DBDataType::INT64] = std::make_pair(PgDataType::Int64,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addValueAsString(result.getInt(), "%lld");
			});

	m_typeHandler[DBDataType::STRING] = std::make_pair(PgDataType::Varchar,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addString(result.getString());
			});

	m_typeHandler[DBDataType::FLOAT] = std::make_pair(PgDataType::Float,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addValueAsString(result.getDouble(), "%f");
			});

	m_typeHandler[DBDataType::DOUBLE] = std::make_pair(PgDataType::Double,
			[](ExecutionResult& result, PgDataWriter& writer) {
				writer.addValueAsString(result.getDouble(), "%f");
			});

	m_typeHandler[DBDataType::DATE] = std::make_pair(PgDataType::Date,
			[](ExecutionResult& result, PgDataWriter& writer) {
				time_t time = result.getInt();
				struct tm* pTime = gmtime(&time);
				if (pTime == nullptr) {
					LOG(ERROR) << "Failed to get gmtime "<< (int ) time;
					writer << nullptr;
				} else {
					writer.addDateTimeAsString(pTime, "%Y-%m-%d", 10);
				}
			});

	m_typeHandler[DBDataType::DATETIME] = std::make_pair(PgDataType::DateTime,
			[](ExecutionResult& result, PgDataWriter& writer) {
				time_t time = result.getInt();
				struct tm* pTime = gmtime(&time);
				if (pTime == nullptr) {
					LOG(ERROR) << "Failed to get gmtime "<< (int ) time;
					writer << nullptr;
				} else {
					writer.addDateTimeAsString(pTime, "%Y-%m-%d %H:%M:%S", 19);
				}
			});
}

size_t PostgresProtocol::buildStartupResponse(std::vector<std::byte>* pBuffer,
		int32_t iSessionIndex) noexcept {
	PgDataWriter writer(pBuffer);
	writeShortMessage(writer, 'R', AUTH_REQ_OK);
	writeShortMessage(writer, 'S', "server_encoding", "UTF8");
	writeShortMessage(writer, 'S', "client_encoding", "UTF8");
	writeShortMessage(writer, 'S', "server_version", "9.0.4");
	writeShortMessage(writer, 'K', iSessionIndex, (int32_t) 0);
	//sync
	writeShortMessage(writer, 'Z', static_cast<int8_t>('I'));

	assert(!writer.isBufferFull());

	return writer.getLastPrepared();
}

size_t PostgresProtocol::buildException(std::vector<std::byte>* pBuffer,
		const std::string& msg, int startPos, bool sync) noexcept {
	PgDataWriter writer(pBuffer);

	writer.begin('E');

	writer << PG_DIAG_SEVERITY << "ERROR" << PG_DIAG_SQLSTATE << "00000"
			<< PG_DIAG_MESSAGE_PRIMARY << msg;

	if (startPos >= 0) {
		std::string pos = std::to_string(startPos);
		writer << PG_DIAG_STATEMENT_POSITION << pos;
	}
	writer << static_cast<int8_t>(0);
	writer.end();
	if (sync) {
		writeShortMessage(writer, 'Z', static_cast<int8_t>('I'));
	}
	if (writer.isBufferFull()) {
		return buildException(pBuffer, msg.substr(0, pBuffer->size() - 100),
				startPos, sync);
	}
	return writer.getLastPrepared();
}

size_t PostgresProtocol::buildColumnDescription(std::vector<std::byte>* pBuffer,
		ExecutionPlan* pPlan, size_t columnNum) {
	PgDataWriter writer(pBuffer);
	writer.begin('T');

	writer << static_cast<int16_t>(columnNum);

	for (size_t i = 0; i < columnNum; ++i) {
		auto sName = pPlan->getProjectionName(i);

		auto dataType = pPlan->getResultType(i);
		if (auto iter = m_typeHandler.find(dataType); iter
				!= m_typeHandler.end()) {
			writer << sName << static_cast<int32_t>(0) //oid
					<< static_cast<int16_t>(i + 1) //column id
					<< static_cast<int32_t>(iter->second.first) //type
					<< static_cast<int16_t>(-1) //datalen
					<< static_cast<int32_t>(-1) //typemod
					<< PARAM_TEXT_MODE;
		} else {
			IO_ERROR("Unexpected data type:",
					DBDataTypeHandler::getTypeName(dataType));
		}
	}

	writer.end();
	if (writer.isBufferFull()) {
		IO_ERROR("not enough network buffer", -1);
	}
	return writer.getLastPrepared();
}

void PostgresProtocol::buildData(PgDataWriter& writer, ExecutionPlan* pPlan,
		size_t columnNum) {
	writer.begin('D');
	writer << static_cast<int16_t>(columnNum);
	for (size_t i = 0; i < columnNum; ++i) {
		try {
			DBDataType type = pPlan->getResultType(i);

			ExecutionResult result;

			pPlan->getResult(i, result, type);

			if (result.isNull()) {
				writer << nullptr;
				continue;
			}
			if (auto iter = m_typeHandler.find(type); iter
					!= m_typeHandler.end()) {
				std::invoke(iter->second.second, result, writer);
			} else {
				IO_ERROR("Unexpected data type:",
						DBDataTypeHandler::getTypeName(type));
			}
		} catch (...) {
			for (; i < columnNum; ++i) {
				writer << nullptr;
			}
			throw;
		}
	} //for
	writer.end();

}
size_t PostgresProtocol::buildExecutionDone(std::vector<std::byte>* pBuffer,
		bool parsed, bool bind, std::string_view sInfo, bool sync) noexcept {
	PgDataWriter writer(pBuffer);

	if (parsed) {
		writer.begin('1');
		writer.end();
	}
	if (bind) {
		writer.begin('2');
		writer.end();
	}
	writeShortMessage(writer, 'C', sInfo);
	if (sync) {
		writeShortMessage(writer, 'Z', static_cast<int8_t>('I'));
	}
	assert(!writer.isBufferFull());
	return writer.getLastPrepared();
}

void PostgresProtocol::readBindParam(MemBuffer* pBuffer, size_t len,
		std::vector<ParamInfo>& params) {
	PgDataReader receiver(pBuffer, len);

	auto portal = receiver.getNextString();
	auto stmt = receiver.getNextString();

	DLOG(INFO)<< "B:portal "<<portal <<" ,stmt "<<stmt;

	size_t iExpectNum = receiver.getNextShort();
	if (iExpectNum != params.size()) {
		IO_ERROR("Parameter format number unmatch!, expect ", iExpectNum,
				", actual ", params.size());
	}

	for (size_t i = 0; i < iExpectNum; ++i) {
		auto type = receiver.getNextShort();
		DLOG(INFO)<< "Bind type:"<<type;
		switch (type) {
		case PARAM_TEXT_MODE:
			params[i].first = false;
			break;
		case PARAM_BINARY_MODE:
			params[i].first = true;
			break;
		default:
			IO_ERROR("Unexpected bind parameter mode: ", type)
			;
		}
	}

	iExpectNum = receiver.getNextShort();
	if (iExpectNum != params.size()) {
		IO_ERROR("Parameter number unmatch!, expect ", iExpectNum, ", actual ",
				params.size());
	}

	for (int i = 0; i < iExpectNum; ++i) {
		params[i].second = receiver.getNextStringWithLen();
	}
}


PostgresProtocol::StartupAction PostgresProtocol::readStartup(
		MemBuffer* pBuffer, size_t len) {
	PgDataReader receiver(pBuffer, len);

	ProtocolVersion proto = (ProtocolVersion) receiver.getNextInt();
	if (proto == CANCEL_REQUEST_CODE) {
		uint32_t iBackendPID = receiver.getNextInt();
		uint32_t iCancelAuthCode = receiver.getNextInt();

		auto pWorker = SessionManager::getInstance().getSession(iBackendPID);
		assert(pWorker);
		pWorker->cancel();
		IO_ERROR("Cancel WorkerID=", iBackendPID);
	}
	if (proto == NEGOTIATE_SSL_CODE) {
		return StartupAction::SSL;
	}
	if (PG_PROTOCOL_MAJOR(proto) < 3) {
		IO_ERROR("Unsupported pg version:", PG_PROTOCOL_MAJOR(proto));
	}
	while (receiver.hasData()) {
		size_t iLen;
		auto sName = receiver.getNextString();
		if (sName == "")
			break;
		auto sValue = receiver.getNextString();

		LOG(INFO)<< "option:" << sName <<"="<< sValue;
	}

	return StartupAction::Normal;
}

