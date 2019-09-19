#include <cassert>
#include <absl/strings/match.h>

#include "SessionManager.h"
#include "IOException.h"
#include "common/MetaConfig.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "common/ParseTools.h"

#include "execution/ExecutionException.h"
#include "execution/BasePlan.h"

#include "planbuilder/BuildPlan.h"

thread_local uint64_t SessionInfo::m_sqlCount;
thread_local uint64_t SessionInfo::m_readCount;
thread_local uint64_t SessionInfo::m_writeCount;

SessionInfo::SessionInfo(int iIndex, asio::io_service& ioService)
	: m_iIndex(iIndex)
	, m_strand(ioService)
	, m_ioService(ioService){
	m_bTerminate.store(false);

	m_handler[0] = &SessionInfo::handleStartup;
	m_handler['S'] = &SessionInfo::handleSync;
	m_handler['Q'] = &SessionInfo::handleQuery;

	m_handler['P'] = &SessionInfo::handleParse;
	m_handler['B'] = &SessionInfo::handleBind;

	m_handler['D'] = &SessionInfo::handleDescription;

	m_handler['E'] = &SessionInfo::handleExecution;
}

void SessionInfo::handleDescription(size_t len) {
	m_protocol.readColumnDescribeInfo(m_pBuffer.get(), len);
	size_t columnNum = 0;
	if (m_pPlan) {
		columnNum = m_pPlan->getResultColumns();
	}

	if (columnNum == 0) {
		readMessageType();
		return;
	}
	auto iWritten = m_protocol.buildColumnDescription(m_pBuffer.get(), m_pPlan.get(), columnNum);
	if(iWritten == 0) {
		IO_ERROR("Not enough network buffer");
	}
	doWrite(iWritten, &SessionInfo::readMessageType);
}

void SessionInfo::handleStartup(size_t len) {
	auto action = m_protocol.readStartup(m_pBuffer.get(), len);

	switch(action) {
	case PostgresProtocol::StartupAction::SSL: {
		DLOG(INFO) <<"Reject SSL connection";
		char* pTarget = reinterpret_cast<char*>(m_pBuffer->data());
		pTarget[0] = 'N';
		doWrite(1, &SessionInfo::readLen);
		break;
	}
	case PostgresProtocol::StartupAction::Normal: {
		size_t iWritten = m_protocol.buildStartupResponse(m_pBuffer.get(), m_iIndex);
		if(iWritten == 0) {
			IO_ERROR("Not enough network buffer");
		}

		doWrite(iWritten, &SessionInfo::readMessageType);
		break;
	}
	default:
		IO_ERROR("Unsupported protocol in startup");
		break;
	}
}

void SessionInfo::handleSync(size_t len) {
	size_t iWritten = m_protocol.buildSync(m_pBuffer.get());
	doWrite(iWritten, &SessionInfo::readMessageType);
}

void SessionInfo::handleQuery(size_t len) {
	auto sql = m_protocol.readQueryInfo(m_pBuffer.get(), len);

	parse(sql);
	resolve();
	m_bParsed = false;
	m_bBinded = false;

	++m_sqlCount;

	size_t columnNum = m_pPlan->getResultColumns();

	if (columnNum > 0) {
		int iWritten = m_protocol.buildColumnDescription(m_pBuffer.get(), m_pPlan.get(), columnNum);
		if(iWritten == 0) {
			IO_ERROR("Not enough network buffer");
		}
		doWrite(iWritten, &SessionInfo::handleExecutionBegin);
	} else {
		handleExecutionBegin();
	}
}

void SessionInfo::handleExecution(size_t len) {
	handleExecutionBegin();
}


void SessionInfo::handleParse(size_t len) {
	assert(m_pResult);
	auto [sql, iParamNum] = m_protocol.readParseInfo(m_pBuffer.get(), len);

	parse(sql);

	++m_sqlCount;

	size_t iActualParamNum = m_pResult->m_bindParamNodes.size();
	if (iParamNum != iActualParamNum) {
		PARSE_ERROR("Parameter number unmatch!, expect ", iParamNum,
				", actual ", iActualParamNum);
	}
	m_bParsed = true;
	m_bBinded = false;

	readMessageType();
}

void SessionInfo::handleBind(size_t len) {
	assert(m_pResult);
	m_pResult->mark();

	size_t iActualNum = m_pResult->m_bindParamNodes.size();

	std::vector<PostgresProtocol::ParamInfo> params(iActualNum);
	m_protocol.readBindParam(m_pBuffer.get(), len, params);

	for(size_t i=0;i<params.size();++i){
		auto pParam = m_pResult->m_bindParamNodes[i];
		pParam->setBindParamMode(params[i].first ? Operation::BINARY_PARAM : Operation::TEXT_PARAM);
		pParam->setString(m_pResult->allocString(params[i].second, false));
	}

	m_bBinded = true;

	resolve();

	readMessageType();
}

void SessionInfo::handleExecutionBegin() {
	m_pPlan->begin(m_bTerminate);
	if(m_pPlan->next(m_bTerminate)) {
		handleExecutionResult();
	} else {
		handleExecutionDone();
	}
}
void SessionInfo::handleExecutionDone()  {
	assert(m_pResult);
	m_pPlan->end();

	auto sInfo = m_pPlan->getInfoString();
	DLOG(INFO)<< "Execute result:" << sInfo;

	m_pPlan = nullptr;
	//discard allocated string for bind param
	m_pResult->restore();

	//need sync for handleQuery, not needed for handleExecution
	auto iWritten = m_protocol.buildExecutionDone(m_pBuffer.get(), m_bParsed, m_bBinded, sInfo, m_qtype == 'Q');

	m_bParsed = false;
	m_bBinded = false;

	doWrite(iWritten, &SessionInfo::readMessageType);
}

void SessionInfo::handleExecutionResult() {
	assert (m_pPlan);
	size_t columnNum = m_pPlan->getResultColumns();


	PgDataWriter writer(m_pBuffer.get());

	do{
		if (columnNum == 0)
			continue;

		m_protocol.buildData(writer, m_pPlan.get(), columnNum);

		if (writer.isBufferFull()) {
			//fresh buffer
			doWrite(writer.getLastPrepared(), &SessionInfo::handleExecutionResult);
			return;
		}
	} while (m_pPlan->next(m_bTerminate));

	doWrite(writer.getLastPrepared(), &SessionInfo::handleExecutionDone);
}

void SessionInfo::cancel() {
	LOG(INFO)<< "Cancel worker-" << m_iIndex;
	m_bTerminate.store(true);
}

void SessionInfo::parse(const std::string_view sql) {
	assert(m_pResult);
	m_pResult->m_pResult = nullptr;
	if (absl::StartsWithIgnoreCase(sql, "DEALLOCATE")) {
		// bypass
	} else if (absl::StartsWithIgnoreCase(sql, "SET ")) {
		// bypass
	} else if (absl::StartsWithIgnoreCase(sql, "BEGIN")) {
		LOG(WARNING)<< "Transaction is not supported";
	} else if (absl::StartsWithIgnoreCase(sql, "COMMIT")) {
		LOG(WARNING) << "Transaction is not supported";
	} else {
		m_pResult->parse(sql);

	}
}

void SessionInfo::resolve() {
	assert(m_pResult);
	auto pTree = m_pResult->m_pResult;
	if (pTree == nullptr) {
		m_pPlan.reset(new EmptyPlan());
	} else {
#ifndef NDEBUG
		printTree(pTree, 0);
#endif
		if (pTree->m_type != NodeType::PLAN) {
			PARSE_ERROR("WRONG NODE ", pTree->m_sExpr);
		}
		m_pPlan = buildPlan(pTree);
	}
}



void SessionInfo::readLen() {
	auto handler = [this](size_t len) {
		uint32_t* pData = reinterpret_cast<uint32_t*>(m_pBuffer->data());
		size_t iLen = ntohl(*pData);
		if (iLen < 4) {
			LOG(ERROR) << "Invalid message length:" << iLen;
			return false;
		}

		iLen -= 4;

		if(m_pBuffer->size() < iLen) {
			LOG(ERROR) << "Not enough network buffer, expect "<< iLen;
			return false;
		}
		readData(iLen);
		return true;
	};

	doRead(sizeof(uint32_t), handler);

}



void SessionInfo::readMessageType() {
	auto handler = [this](size_t len) {
		m_qtype = static_cast<char>(m_pBuffer->at(0));
		if (m_qtype == 'X' || m_qtype == 0) {
			DLOG(INFO)<< "Client Terminate!";
			return false;
		}

		readLen();
		return true;
	};

	doRead(1,handler);
}

void SessionInfo::readData(size_t len) {
	auto handler = [this](size_t len) {
		if(auto iter = m_handler.find(m_qtype); iter != m_handler.end()) {
			size_t iWritten = 0;
			try {
				std::invoke(iter->second, this, len);
			} catch (ParseException& e) {
				if(m_qtype == 0) {
					//close session if startup failed
					return false;
				}
				iWritten = m_protocol.buildException(m_pBuffer.get(), e.what(), e.getStartPos(), m_qtype == 'Q');
			} catch (std::exception& e) {
				if(m_qtype == 0) {
					//close session if startup failed
					return false;
				}
				iWritten = m_protocol.buildException(m_pBuffer.get(), e.what(), -1, m_qtype == 'Q');
			}
			if(iWritten > 0) {
				doWrite(iWritten, &SessionInfo::readMessageType);
			}
			return true;
		 }else {
			LOG(ERROR) << "Unknown message "<< m_qtype << ", len=" <<len;
			return false;
		}
	};
	doRead(len,handler);

}

void SessionInfo::start() {
	m_qtype = 0;
	m_pResult.emplace();
	m_pBuffer = GlobalMemBlockPool::getInstance().alloc();
	readLen();
}

void SessionInfo::end() {
	m_pSocket.reset();
	m_pResult.reset();
	GlobalMemBlockPool::getInstance().free(std::move(m_pBuffer));
	SessionManager::getInstance().freeSession(this);
}

void SessionInfo::doRead(size_t len, std::function<bool(size_t len)> fn) {
	if(len == 0 ) {
		if(!fn(0)) {
			end();
		}
		return;
	}
	auto handler = [expectLen = len, this, fn](const std::error_code& ex, size_t len) {
		++m_readCount;
		if(ex) {
			LOG(ERROR) << "read failed: " << ex.message();
			end();
			return;
		}
		assert(expectLen == len);
		if(!fn(len)) {
			end();
		}
	};
	asio::async_read(*m_pSocket, asio::buffer((void*) m_pBuffer->data(), len), m_strand.wrap(handler));
}

void SessionInfo::doWrite(size_t len, void (SessionInfo::*fn)()) {
	auto handler = [actualLen = len, this, fn](const std::error_code& ex, size_t len) {
		++m_writeCount;
		if(ex) {
			LOG(ERROR) << "write failed: " << ex.message();
			end();
			return;
		}
		assert(actualLen == len);
		try {
			std::invoke(fn, this);
		} catch (std::exception& e) {
			if(m_qtype == 0) {
				end();
				return;
			}
			auto iWritten = m_protocol.buildException(m_pBuffer.get(), e.what(), -1, m_qtype == 'Q');
			doWrite(iWritten, &SessionInfo::readMessageType);
		}
	};
	if(len == 0) {
		handler(std::error_code{}, len);
	} else {
		asio::async_write(*m_pSocket, asio::buffer((void*) m_pBuffer->data(), len), m_strand.wrap(handler));
	}
}


