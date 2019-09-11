#include "MessageSender.h"
#include "common/ConfigInfo.h"

namespace {


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

}

decltype(MessageSender::m_typeHandler) MessageSender::m_typeHandler;

void MessageSender::init() {
	m_typeHandler[DBDataType::BYTES] = std::make_pair(PgDataType::Bytea, [](ExecutionResult& result, DataSender& sender) {
		sender.addBytesString(result.getString());
	});

	m_typeHandler[DBDataType::INT16] = std::make_pair(PgDataType::Int16, [](ExecutionResult& result, DataSender& sender) {
		sender.addValueAsString(result.getInt(), "%lld");
	});

	m_typeHandler[DBDataType::INT32] = std::make_pair(PgDataType::Int32, [](ExecutionResult& result, DataSender& sender) {
		sender.addValueAsString(result.getInt(), "%lld");
	});

	m_typeHandler[DBDataType::INT64] = std::make_pair(PgDataType::Int64, [](ExecutionResult& result, DataSender& sender) {
		sender.addValueAsString(result.getInt(), "%lld");
	});

	m_typeHandler[DBDataType::STRING] = std::make_pair(PgDataType::Varchar, [](ExecutionResult& result, DataSender& sender) {
		sender.addString(result.getString());
	});

	m_typeHandler[DBDataType::FLOAT] = std::make_pair(PgDataType::Float, [](ExecutionResult& result, DataSender& sender) {
		sender.addValueAsString(result.getDouble(), "%f");
	});

	m_typeHandler[DBDataType::DOUBLE] = std::make_pair(PgDataType::Double, [](ExecutionResult& result, DataSender& sender) {
		sender.addValueAsString(result.getDouble(), "%f");
	});

	m_typeHandler[DBDataType::DATE] = std::make_pair(PgDataType::Date, [](ExecutionResult& result, DataSender& sender) {
		time_t time = result.getInt();
		struct tm* pTime = gmtime(&time);
		if (pTime == nullptr) {
			LOG(ERROR) << "Failed to get gmtime "<< (int ) time;
			sender.addInt(-1);
		} else {
			sender.addDateTimeAsString(pTime, "%Y-%m-%d", 10);
		}
	});

	m_typeHandler[DBDataType::DATETIME] = std::make_pair(PgDataType::DateTime, [](ExecutionResult& result, DataSender& sender) {
		time_t time = result.getInt();
		struct tm* pTime = gmtime(&time);
		if (pTime == nullptr) {
			LOG(ERROR) << "Failed to get gmtime "<< (int ) time;
			sender.addInt(-1);
		} else {
			sender.addDateTimeAsString(pTime, "%Y-%m-%d %H:%M:%S", 19);
		}
	});
}
void MessageSender::sendException(Exception* pe) {
	std::string msg = pe->what();

	m_sender<< PG_DIAG_SEVERITY << "ERROR"
			<< PG_DIAG_SQLSTATE << "00000"
			<< PG_DIAG_MESSAGE_PRIMARY << msg;

	if (pe->getLine() >= 0) {
		std::string pos = std::to_string(pe->getStartPos());
		m_sender << PG_DIAG_STATEMENT_POSITION << pos;
	}
	m_sender << static_cast<int8_t>(0);
}


void MessageSender::sendColumnDescription(ExecutionPlan* pPlan, size_t columnNum) {
	assert(columnNum > 0);
	m_sender.addShort(columnNum);

	for (size_t i = 0; i < columnNum; ++i) {
		auto sName = pPlan->getProjectionName(i);

		auto dataType = pPlan->getResultType(i);
		if(auto iter = m_typeHandler.find(dataType); iter != m_typeHandler.end()) {
			addDataTypeMsg(sName, i + 1, iter->second.first, -1);
		} else {
			IO_ERROR("Unexpected data type:", GetTypeName(dataType));
		}
	}
}

void MessageSender::sendData(ExecutionPlan* pPlan) {
	size_t columnNum;
	if (pPlan == nullptr) {
		columnNum = 0;
	} else {
		columnNum = pPlan->getResultColumns();
	}

	m_sender.addShort(columnNum);
	for (size_t i = 0; i < columnNum; ++i) {
		try {
			DBDataType type = pPlan->getResultType(i);

			ExecutionResult result;

			pPlan->getResult(i, result, type);


			if (result.isNull()) {
				m_sender.addInt(-1);
				continue;
			}
			if(auto iter = m_typeHandler.find(type); iter != m_typeHandler.end()) {
				std::invoke(iter->second.second, result, m_sender);
			} else {
				IO_ERROR("Unexpected data type:", GetTypeName(type));
			}
		} catch (...) {
			for (; i < columnNum; ++i)
				m_sender.addInt(-1);
			throw;
		}
	} //for
}
