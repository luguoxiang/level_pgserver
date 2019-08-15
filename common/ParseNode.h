#pragma once

#include <vector>
#include <cassert>
#include <stdlib.h>
#include <string>
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

class ParseNode;
struct ParseResult;

using BuildPlanFunc = void (*)(ParseNode* pNode);

class ParseNode {
public:
    ParseNode(ParseResult* p, NodeType type, int num);

	NodeType m_type;
	std::string m_sValue;
	int64_t m_iValue;
	std::string m_sExpr;

	BuildPlanFunc m_fnBuildPlan;
    void remove(const std::string sNewName, const std::string sRemove);

    const size_t children() const {return m_children.size();}

	std::vector<ParseNode*> m_children;


private:
    bool _collect(std::vector<ParseNode*>& children, const std::string sRemove);

};

constexpr size_t MAX_ERROR_MSG = 200;

struct ParseResult {
	std::string m_sSql;
	void* m_scanInfo;
	ParseNode* m_pResult;
	char m_szErrorMsg[MAX_ERROR_MSG];
	int m_iStartCol;
	int m_iEndCol;
	int m_iLine;
	int m_yycolumn;
	int m_yylineno;

	std::vector<std::unique_ptr<ParseNode>> m_nodes;
};

int64_t parseTime(const char* pszTime);

int parseInit(ParseResult* p);

int parseTerminate(ParseResult* p);

void parseSql(ParseResult *p, const std::string& sql);

void printTree(ParseNode* pRoot, int level);

std::string cleanString(const char* pszSrc);
std::string parseBinary(const char* pszSrc);


ParseNode* newFuncNode(ParseResult *p, const std::string& sName, int firstColumn,
		int lastColumn, int num, ...);

ParseNode* newExprNode(ParseResult *p, int value, int firstColumn,
	int lastColumn, int num, ...);

ParseNode* newIntNode(ParseResult *p, NodeType type, int value, int num, ...);

ParseNode* newParentNode(ParseResult *p, const std::string& sName, int num, ...);

