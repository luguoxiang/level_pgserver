#ifndef PARSENODE_H
#define PARSENODE_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define INT_NODE 0
#define STR_NODE 1
#define BINARY_NODE 2
#define DATE_NODE 3
#define FLOAT_NODE 4
#define BOOL_NODE 5 
#define NULL_NODE 6
#define HINT_NODE 7
#define PARENT_NODE 15
#define NAME_NODE 16
#define OP_NODE 17 
#define FUNC_NODE 18 
#define DATATYPE_NODE 19 
#define INFO_NODE 20 
#define PARAM_NODE 21 

#define OP_CODE(pNode) (pNode->m_iValue)
#define FUNC_CODE(pNode) (pNode->m_iValue)

#define BUILD_PLAN(pNode) \
	if(pNode) pNode->m_fnBuildPlan(pNode)

#define SQL_SELECT_PROJECT 0
#define SQL_SELECT_TABLE 1
#define SQL_SELECT_PREDICATE 2
#define SQL_SELECT_GROUPBY 3
#define SQL_SELECT_HAVING 4
#define SQL_SELECT_ORDERBY 5
#define SQL_SELECT_LIMIT 6

struct _ParseNode;

typedef void (*BuildPlanFunc)(struct _ParseNode* pNode);

#define POOL_BLOCK_SIZE 1024 * 32

typedef struct _MemoryPoolBlock
{
	char m_szData[POOL_BLOCK_SIZE];
	struct _MemoryPoolBlock* m_pNext;
	size_t m_iUsed;
} MemoryPoolBlock;

typedef struct _ParseNode
{
	int m_iType;
	const char* m_pszValue;
	int64_t m_iValue;
	const char* m_pszExpr;

	struct _ParseNode** m_children;
	size_t m_iChildNum;
	BuildPlanFunc m_fnBuildPlan;
} ParseNode;

#define MAX_ERROR_MSG 200

typedef struct _ParseResult
{
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
} ParseResult;

#ifdef __cplusplus
extern "C"
{
#endif

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
#ifdef __cplusplus
}
#endif

#endif //PARSENODE_H
