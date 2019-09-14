#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include <optional>

#include "common/ParseResult.h"
#include "common/QueryRewritter.h"
#include "PostgresProtocol.h"

class WorkThreadInfo {
public:
	WorkThreadInfo(int iIndex, const std::atomic_bool& bGlobalTerminate);
	~WorkThreadInfo();

	void cancel(bool planOnly);

	void run(int fd);

	int16_t getIndex() {return m_iIndex;}
	bool isRunning() {return m_bRunning; }

	int64_t getSqlCount() {return m_iSqlCount;}
	int64_t getClientTime() {return m_iClientTime / 1000;}
	int64_t getSessions() {return m_iSessions;}

	WorkThreadInfo(const WorkThreadInfo&) = delete;
	WorkThreadInfo& operator =(const WorkThreadInfo&) = delete;
private:
	bool m_bRunning = false;
	uint64_t m_iClientTime = 0;
	int32_t m_iSessions = 0;

	void parse(const std::string_view sql);

	std::string_view allocString(std::string_view s) {
		size_t len = s.length();
		char* alloc = m_result.alloc(len);

		std::copy(s.data(), s.data() + len, alloc);
		return std::string_view(alloc, len);
	}

	size_t getBindParamNumber() {
		return m_result.m_bindParamNodes.size();
	}

	ParseNode* getBindParam(size_t i) {
		assert(i < getBindParamNumber());
		return m_result.m_bindParamNodes[i];
	}

	int m_iIndex;
	int m_iSqlCount = 0;

	int m_iAcceptFd = 0;

	ParseResult m_result;
	QueryRewritter m_rewritter;

	void resolve();
	void sendRow();

	void handleQuery();
	void handleParse();
	void handleBind();
	void handleExecute();

	std::optional<PostgresProtocol> m_protocol;

	WorkThreadInfo* m_pWorker;

	uint64_t m_iSendTime = 0;

	std::map<char, std::function<void ()>> m_handler;

	ExecutionPlanPtr m_pPlan;

	const std::atomic_bool& m_bGlobalTerminate;

	std::atomic_bool m_bTerminate;
};


