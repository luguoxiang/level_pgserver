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
	PARENT,
	NAME,
	OP,
	FUNC,
	INFO,
	PARAM,
	LIST,
	PLAN,
};

enum class Operation {
 NONE,
 COMP_EQ,
 COMP_NE,
 COMP_LE,
 COMP_LT,
 COMP_GT,
 COMP_GE,
 LIKE,
 IN,
 NOT_IN,

 MINUS,
 ADD,
 SUB,
 DIV,
 MUL,
 MOD,

 MEMBER,
 AS,

 AND,
 OR,

 ASC,
 DESC,

 ALL_COLUMNS,

 SHOW_TABLES,
 DESC_TABLE,
 WORKLOAD,
 SELECT,
 INSERT,
 DELETE,
 EXPLAIN,
 UNION_ALL,
 VALUES,
 SELECT_WITH_SUBQUERY,
};

constexpr int16_t PARAM_TEXT_MODE = 0;
constexpr int16_t PARAM_BINARY_MODE = 1;



class ParseNode {
public:
	ParseNode(NodeType type,
			Operation op,
			const std::string_view sExpr,
			size_t childNum,
			const ParseNode** children);

	const NodeType m_type;
	std::string_view m_sValue;
	int64_t m_iValue = 0;
	const Operation m_op;

	//string view on ParseResult.m_sSql
	const std::string_view m_sExpr;

    const size_t children() const {return m_iChildNum;}

    const ParseNode* getChild(size_t i) const {
    	assert(i < m_iChildNum);
    	return m_children[i];
    }
    bool isConst() const {
    	switch(m_type) {
    	case NodeType::INT:
    	case NodeType::STR:
    	case NodeType::BINARY:
    	case NodeType::DATE:
    	case NodeType::FLOAT:
    	case NodeType::BOOL:
    	case NodeType::PARAM:
    		return true;
    	default:
    		return false;
    	}
    }
private:
    const ParseNode** m_children;
    size_t m_iChildNum;
};


inline bool IS_DIGIT(char c) {
	return c >='0' && c<='9';
}

int64_t parseTime(std::string_view sTime);

void printTree(const ParseNode* pRoot, int level);


