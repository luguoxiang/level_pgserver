#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <cassert>
#include <stdlib.h>
#include <string>
#include <variant>
#include <initializer_list>


enum class NodeType {
	INT = 0,
	STR,
	BINARY,
	DATE,
	FLOAT,
	NONE,
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
 VALUES,
 SELECT_WITH_SUBQUERY,

 TEXT_PARAM,
 BINARY_PARAM,
 UNBOUND_PARAM,
};



class ParseNode {
public:
	ParseNode(NodeType type,
			Operation op,
			const std::string_view sExpr,
			size_t childNum,
			ParseNode** children);

	const NodeType m_type;

	//string view on ParseResult.m_sSql
	const std::string_view m_sExpr;

    const size_t children() const {return m_iChildNum;}

    ParseNode* getChild(size_t i) const {
    	assert(i < m_iChildNum);
    	return m_children[i];
    }

    void setChild(size_t i,  ParseNode* pChild) {
    	assert(i < m_iChildNum);
    	m_children[i] = pChild;
    }
    bool isConst() const {
    	switch(m_type) {
    	case NodeType::INT:
    	case NodeType::STR:
    	case NodeType::BINARY:
    	case NodeType::DATE:
    	case NodeType::FLOAT:
    	case NodeType::PARAM:
    		return true;
    	default:
    		return false;
    	}
    }
    bool isTrueConst() const {
    	switch(m_type) {
    	case NodeType::INT:
    		return getInt() != 0;
    	default:
    		return false;
    	}
    }

    bool isFalseConst() const {
    	switch(m_type) {
    	case NodeType::INT:
    		return getInt() == 0;
    	default:
    		return false;
    	}
    }

    Operation setBindParamMode(Operation op) {
    	assert(m_type == NodeType::PARAM);
    	m_op = op;
    	return m_op;
    }
    Operation getOp() const {
    	return m_op;
    }

    void setInt(int64_t value) {m_value = value;}
    int64_t getInt() const {return std::get<int64_t>(m_value);}

    void setDouble(double value) {m_value = value;}
    double getDouble() const {return std::get<double>(m_value);}

    void setString(const std::string_view value) {m_value = value;}
    const std::string_view getString() const {return std::get<std::string_view>(m_value);}
private:
    std::variant<int64_t, double, std::string_view> m_value;

	Operation m_op;
    ParseNode** m_children;
    size_t m_iChildNum;
};


inline bool IS_DIGIT(char c) {
	return c >='0' && c<='9';
}

int64_t parseTime(const char* pszTime);

void printTree(const ParseNode* pRoot, int level);


