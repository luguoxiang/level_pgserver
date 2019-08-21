#include <cassert>
#include <limits>

#include "WorkThreadInfo.h"
#include "common/ParseException.h"
#include "common/ParseNode.h"
#include "execution/ExecutionException.h"
#include "execution/ParseTools.h"

thread_local WorkThreadInfo* WorkThreadInfo::m_pWorkThreadInfo = nullptr;

constexpr size_t EXECUTION_BUFFER_BLOCK_SIZE = 512 * 1024;
constexpr size_t PARSE_BUFFER_BLOCK_SIZE = 5 * 1024;
constexpr size_t PARSE_BUFFER_SIZE = 1024 * 1024;

WorkThreadInfo::WorkThreadInfo(int fd, int port, int iIndex, size_t executionBufferSize) :
		m_iListenFd(fd), m_port(port), m_iIndex(iIndex),
		m_executionBuffer(executionBufferSize, EXECUTION_BUFFER_BLOCK_SIZE),
		m_parseBuffer(PARSE_BUFFER_SIZE, PARSE_BUFFER_BLOCK_SIZE){

	m_result = {};
	m_result.allocStringFn = [this] (const char* p, size_t size) {
		return  m_parseBuffer.allocString(p, size);
	};
	m_result.allocParseNodeFn = [this]  (NodeType type,
				const std::string_view sExpr,
				std::vector<const ParseNode*> children) {
		const ParseNode** ppChild = nullptr;
		if (children.size() > 0) {
			ppChild = reinterpret_cast<const ParseNode**>(m_parseBuffer.doAlloc(children.size() * sizeof(const ParseNode*)));
			size_t index = 0;
			for (size_t i =0;i<children.size();++i) {
				ppChild[i] = children[i];
			}
		}
		ParseNode* pNode = new(m_parseBuffer.doAlloc(sizeof(ParseNode))) ParseNode(type, sExpr, children.size(), ppChild);

		return pNode;
	};
	if (parseInit(&m_result)) {
		throw new ParseException("Failed to init parser!");
	}
}

WorkThreadInfo::~WorkThreadInfo() {
	parseTerminate(&m_result);
	m_plans.clear();
}

void WorkThreadInfo::parse(const std::string_view sql) {

	m_parseBuffer.purge();

	parseSql(&m_result, sql);

	if (m_result.m_pResult == 0) {
		throw new ParseException(&m_result);
	}
}

void WorkThreadInfo::print() {
	assert(m_result.m_pResult);
	printTree(m_result.m_pResult, 0);
}

ExecutionPlan* WorkThreadInfo::resolve() {
	assert(m_result.m_pResult);

	BUILD_PLAN(m_result.m_pResult);

	ExecutionPlan* pPlan = popPlan();
	assert(pPlan && m_plans.empty());
	return pPlan;
}


WorkerManager& WorkerManager::getInstance() {
	static WorkerManager manager;
	return manager;
}



