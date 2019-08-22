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
constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;

constexpr int SQL_SELECT_PROJECT = 0;
constexpr int SQL_SELECT_TABLE = 1;
constexpr int SQL_SELECT_PREDICATE = 2;
constexpr int SQL_SELECT_GROUPBY = 3;
constexpr int SQL_SELECT_HAVING = 4;
constexpr int SQL_SELECT_ORDERBY = 5;
constexpr int SQL_SELECT_LIMIT = 6;



class ParseNode;

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

    const size_t children() const {return m_iChildNum;}

    const ParseNode* getChild(size_t i) const {
    	assert(i < m_iChildNum);
    	return m_children[i];
    }

private:
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

int64_t parseTime(std::string_view sTime);

void printTree(const ParseNode* pRoot, int level);


