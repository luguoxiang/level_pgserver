#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <cassert>
#include <stdlib.h>
#include <string>
#include <initializer_list>

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

constexpr int SQL_SELECT_PROJECT = 0;
constexpr int SQL_SELECT_TABLE = 1;
constexpr int SQL_SELECT_PREDICATE = 2;
constexpr int SQL_SELECT_GROUPBY = 3;
constexpr int SQL_SELECT_HAVING = 4;
constexpr int SQL_SELECT_ORDERBY = 5;
constexpr int SQL_SELECT_LIMIT = 6;

template<typename ...Args>
inline std::string ConcateToString(Args&&...args) {
        std::ostringstream os;
        (os << ... << std::forward<Args>(args));
        return os.str();
}

class ParseNode;
struct ParseResult;

using BuildPlanFunc = void (*)(ParseNode* pNode);

class ParseNode {
public:
    ParseNode(ParseResult* p,
    		NodeType type,
			int firstColumn,
    		int lastColumn,
			std::initializer_list<ParseNode*> children = {});

    ParseNode(ParseResult* p,
    		NodeType type,
			const std::string_view sExpr,
			std::initializer_list<ParseNode*> children = {});
	NodeType m_type;
	std::string m_sValue;
	int64_t m_iValue = 0;

	//string view on ParseResult.m_sSql
	const std::string_view m_sExpr;

	BuildPlanFunc m_fnBuildPlan;
	ParseNode* merge(ParseResult* p, const std::string sNewName, const std::string sRemove);

    const size_t children() const {return m_children.size();}

	std::vector<ParseNode*> m_children;


private:
    bool _collect(std::vector<ParseNode*>& children, const std::string sRemove);

};

inline int OP_CODE(ParseNode* pNode) {return pNode->m_iValue;}
inline int FUNC_CODE(ParseNode* pNode) { return pNode->m_iValue;}

inline void BUILD_PLAN(ParseNode* pNode) {
	if(pNode) pNode->m_fnBuildPlan(pNode);
}


struct ParseResult {
	std::string m_sSql;
	void* m_scanInfo;
	ParseNode* m_pResult;
	std::string m_sError;
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

void parseSql(ParseResult *p, const std::string_view sql);

void printTree(ParseNode* pRoot, int level);

std::string cleanString(const char* pszSrc);
std::string parseBinary(const char* pszSrc);


inline ParseNode* newFuncNode(ParseResult *p, const std::string& sName, int firstColumn,
		int lastColumn,
		std::initializer_list<ParseNode*> children) {
	ParseNode* pNode = new ParseNode(p, NodeType::FUNC,firstColumn, lastColumn, children);
	pNode->m_sValue = sName;
	return pNode;
}

inline ParseNode* newExprNode(ParseResult *p, int value, int firstColumn,
	int lastColumn, std::initializer_list<ParseNode*> children) {
	ParseNode* pNode = new ParseNode(p, NodeType::OP,firstColumn, lastColumn, children);
	pNode->m_iValue = value;
	return pNode;
}

inline ParseNode* newInfoNode(ParseResult *p, int value,
		int firstColumn, int lastColumn) {
	ParseNode* pNode = new ParseNode(p, NodeType::INFO,  firstColumn, lastColumn, {});
	pNode->m_iValue = value;
	return pNode;
}

inline ParseNode* newParentNode(ParseResult *p, const std::string& sName,
		int firstColumn, int lastColumn,
		std::initializer_list<ParseNode*> children) {
	ParseNode* pNode = new ParseNode(p, NodeType::PARENT,  firstColumn, lastColumn, children);
	pNode->m_sValue = sName;
	return pNode;
}

