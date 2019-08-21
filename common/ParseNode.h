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

using BuildPlanFunc = void (*)(const ParseNode* pNode);

class ParseNode {
public:
	ParseNode(NodeType type,
			const std::string_view sExpr,
			size_t childNum,
			const ParseNode** children);

	NodeType m_type;
	std::string_view m_sValue;
	int64_t m_iValue;

	//string view on ParseResult.m_sSql
	std::string_view m_sExpr;

	BuildPlanFunc m_fnBuildPlan;
	ParseNode* merge(ParseResult* p, const std::string sNewName, const std::string sRemove) const;

    const size_t children() const {return m_iChildNum;}

    const ParseNode* getChild(size_t i) const {
    	assert(i < m_iChildNum);
    	return m_children[i];
    }

private:
    bool _collect(std::vector<const ParseNode*>& children, const std::string sRemove) const;
    const ParseNode** m_children;
    size_t m_iChildNum;
};

inline int OP_CODE(const ParseNode* pNode) {return pNode->m_iValue;}
inline int FUNC_CODE(const ParseNode* pNode) { return pNode->m_iValue;}

inline void BUILD_PLAN(const ParseNode* pNode) {
	if(pNode) pNode->m_fnBuildPlan(pNode);
}

inline bool IS_DIGIT(char c) {
	return c >='0' && c<='9';
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

	using AllocString = std::function<char*(const char* , size_t)>;
	using AllocParseNode = std::function<ParseNode* (NodeType type,
			const std::string_view sExpr,
			std::vector<const ParseNode*> children)>;

	AllocString allocStringFn;
	AllocParseNode allocParseNodeFn;
};

int64_t parseTime(std::string_view sTime);

int parseInit(ParseResult* p);

int parseTerminate(ParseResult* p);

void parseSql(ParseResult *p, const std::string_view sql);

void printTree(const ParseNode* pRoot, int level);

ParseNode* newParseNode(ParseResult* p,
		NodeType type,
		const std::string_view sExpr,
		std::initializer_list<const ParseNode*> children = {});

inline ParseNode* newParseNode(ParseResult* p,
		NodeType type,
		int firstColumn,
		int lastColumn,
		std::initializer_list<const ParseNode*> children = {}) {
	std::string_view sExpr(p->m_sSql.c_str() + firstColumn - 1,lastColumn - firstColumn + 1);
	return newParseNode(p, type, sExpr, children);
}

inline ParseNode* newFuncNode(ParseResult *p, const std::string_view& sName, int firstColumn,
		int lastColumn,
		std::initializer_list<const ParseNode*> children) {
	ParseNode* pNode = newParseNode(p, NodeType::FUNC,firstColumn, lastColumn, children);
	pNode->m_sValue = sName;
	return pNode;
}

inline ParseNode* newExprNode(ParseResult *p, int value, int firstColumn,
	int lastColumn, std::initializer_list<const ParseNode*> children) {
	ParseNode* pNode = newParseNode(p, NodeType::OP,firstColumn, lastColumn, children);
	pNode->m_iValue = value;
	return pNode;
}

inline ParseNode* newInfoNode(ParseResult *p, int value,
		int firstColumn, int lastColumn) {
	ParseNode* pNode = newParseNode(p, NodeType::INFO,  firstColumn, lastColumn, {});
	pNode->m_iValue = value;
	return pNode;
}

inline ParseNode* newParentNode(ParseResult *p, const std::string_view& sName,
		int firstColumn, int lastColumn,
		std::initializer_list<const ParseNode*> children) {
	ParseNode* pNode = newParseNode(p, NodeType::PARENT,  firstColumn, lastColumn, children);
	pNode->m_sValue = sName;
	return pNode;
}

