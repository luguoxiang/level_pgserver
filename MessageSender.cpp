#include "MessageSender.h"


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

		switch (pPlan->getResultType(i)) {
		case DBDataType::BYTES:
			addDataTypeMsg(sName, i + 1, PgDataType::Bytea, -1, false);
			break;
		case DBDataType::INT8:
		case DBDataType::INT16:
			addDataTypeMsg(sName, i + 1, PgDataType::Int16, 2, false);
			break;
		case DBDataType::INT32:
			addDataTypeMsg(sName, i + 1, PgDataType::Int32, 4, false);
			break;
		case DBDataType::INT64:
			addDataTypeMsg(sName, i + 1, PgDataType::Int64, 8, false);
			break;
		case DBDataType::STRING:
			addDataTypeMsg(sName, i + 1, PgDataType::Varchar, -1, false);
			break;
		case DBDataType::DATETIME:
			addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::DATE:
			addDataTypeMsg(sName, i + 1, PgDataType::DateTime, -1, false);
			break;
		case DBDataType::FLOAT:
			addDataTypeMsg(sName, i + 1, PgDataType::Float, -1, false);
			break;
		case DBDataType::DOUBLE:
			addDataTypeMsg(sName, i + 1, PgDataType::Double, -1, false);
			break;
		default:
			LOG(ERROR) << "Unknown type for " << sName;
			assert(0);
			break;
		}
	}
}

void MessageSender::sendData(ExecutionPlan* pPlan, RowDataSender& dataSender) {
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

			switch (type) {
			case DBDataType::BYTES: {
				dataSender.addBytes(result.getString());
				continue;
			}
			case DBDataType::STRING:
				dataSender.addString(result.getString());
				continue;
			case DBDataType::INT8:
			case DBDataType::INT16:
				dataSender.addInt16(result.getInt());
				break;
			case DBDataType::INT32:
				dataSender.addInt32(result.getInt());
				break;
			case DBDataType::INT64:
				dataSender.addInt64(result.getInt());
				break;
			case DBDataType::DATETIME:
			case DBDataType::DATE: {
				time_t time = result.getInt();
				struct tm* pToday = gmtime(&time);
				if (pToday == nullptr) {
					LOG(ERROR) << "Failed to get localtime "<< (int ) time;
					m_sender.addInt(-1);
				} else if(type == DBDataType::DATE){
					dataSender.addDate(pToday, time);
				} else {
					dataSender.addDateTime(pToday, time);
				}
				break;
			}
			case DBDataType::FLOAT:
				dataSender.addFloat(result.getDouble());
				break;
			case DBDataType::DOUBLE: {
				dataSender.addDouble(result.getDouble());
				break;
			}
			default:
				assert(0);
				break;
			}; //switch
		} catch (...) {
			for (; i < columnNum; ++i)
				m_sender.addInt(-1);
			throw;
		}
	} //for
}
