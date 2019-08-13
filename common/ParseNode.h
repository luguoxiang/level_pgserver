#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

enum class NodeType {
	INT = 0,
	STR,
	BINARY,
	DATE,
	FLOAT,
	BOOL,
	NONE,
	HINT,
	PARENT,
	NAME,
	OP,
	FUNC,
	DATATYPE,
	INFO,
	PARAM,
};

#define OP_CODE(pNode) (pNode->m_iValue)
#define FUNC_CODE(pNode) (pNode->m_iValue)

#define BUILD_PLAN(pNode) \
	if(pNode) pNode->m_fnBuildPlan(pNode)

constexpr int SQL_SELECT_PROJECT = 0;
constexpr int SQL_SELECT_TABLE = 1;
constexpr int SQL_SELECT_PREDICATE = 2;
constexpr int SQL_SELECT_GROUPBY = 3;
constexpr int SQL_SELECT_HAVING = 4;
constexpr int SQL_SELECT_ORDERBY = 5;
constexpr int SQL_SELECT_LIMIT = 6;

struct ParseNode;

using BuildPlanFunc = void (*)(ParseNode* pNode);

constexpr size_t POOL_BLOCK_SIZE = 1024 * 32;

struct MemoryPoolBlock {
	char m_szData[POOL_BLOCK_SIZE];
	MemoryPoolBlock* m_pNext;
	size_t m_iUsed;
};

struct ParseNode {
	NodeType m_iType;
	const char* m_pszValue;
	int64_t m_iValue;
	const char* m_pszExpr;

	ParseNode** m_children;
	size_t m_iChildNum;
	BuildPlanFunc m_fnBuildPlan;
};

constexpr size_t MAX_ERROR_MSG = 200;

struct ParseResult {
	const char* m_pszSql;
	void* m_scanInfo;
	ParseNode* m_pResult;
	char m_szErrorMsg[MAX_ERROR_MSG];
	int m_iStartCol;
	int m_iEndCol;
	int m_iLine;
	int m_yycolumn;
	int m_yylineno;
	MemoryPoolBlock* m_pPoolHead;
	MemoryPoolBlock* m_pPoolTail;
};

extern int64_t parseTime(const char* pszTime);

extern int parseInit(ParseResult* p);

extern int parseTerminate(ParseResult* p);

extern void parseSql(ParseResult *p, const char* pszSql, size_t iLen);

extern void printTree(ParseNode* pRoot, int level);

extern char* memPoolAlloc(size_t iSize, ParseResult *p);

extern void memPoolClear(ParseResult *p, int8_t bReleaseAll);

extern size_t memPoolUsed(ParseResult *p);

extern char* my_strdup(ParseResult *p, const char* pszSrc);

extern char* my_memdup(ParseResult *p, const char* pszSrc, size_t len);

extern size_t parseString(const char* pszSrc, char* pszDest, size_t iLen);

