#pragma once

#include "ParseTools.h"
#include "common/ParseException.h"
#include <string>
#include "common/Log.h"

/*
 * Base class to implement parse tree traverse behavior.
 * Note that walk(pNode) method does not visit pNode's child node,
 * It is caller's responsibility to call walk() on child nodes in each visit method.
 */
template<typename ParamType, typename ReturnType>
class ParseTreeVisitor {
public:
	virtual ~ParseTreeVisitor() {
	}

	virtual ReturnType visitDataNode(ParseNode* pNode, ParamType param) {
		throw new ParseException("Unimplemented method");
	}
	virtual ReturnType visitNameNode(ParseNode* pNode, ParamType param) {
		throw new ParseException("Unimplemented method");
	}
	virtual ReturnType visitDyadicOpNode(int op, ParseNode* pNode,
			ParamType param) {
		throw new ParseException("Unimplemented method");
	}
	virtual ReturnType visitUnaryOpNode(int op, ParseNode* pNode,
			ParamType param) {
		throw new ParseException("Unimplemented method");
	}
	virtual ReturnType visitFuncNode(const char* pszName, ParseNode* pNode,
			ParamType param) {
		throw new ParseException("Unimplemented method");
	}

	ReturnType walk(ParseNode* pRoot, ParamType param);

	static inline const char* getOpString(int opcode);
};

template<typename ParamType, typename ReturnType>
const char*
ParseTreeVisitor<ParamType, ReturnType>::getOpString(int opcode) {
	switch (opcode) {
	case LIKE:
		return "like";
	case COMP_GE:
		return ">=";
	case COMP_GT:
		return ">";
	case COMP_LE:
		return "<=";
	case COMP_LT:
		return "<";
	case COMP_EQ:
		return "=";
	case COMP_NE:
		return "!=";
	case ANDOP:
		return "and";
	case OR:
		return "or";
	case '+':
		return "+";
	case '-':
		return "-";
	case '*':
		return "*";
	case '/':
		return "/";
	default:
		LOG(ERROR, "Unsupported expression operator %d!", opcode)
		;
		throw new ParseException("Unsupported expression operator %d!", opcode);
	}
}

template<typename ParamType, typename ReturnType>
ReturnType ParseTreeVisitor<ParamType, ReturnType>::walk(ParseNode* pNode,
		ParamType param) {
	switch (pNode->m_iType) {
	case NAME_NODE:
		return visitNameNode(pNode, param);

	case STR_NODE:
	case DATE_NODE:
	case NULL_NODE:
	case INT_NODE:
	case FLOAT_NODE:
	case BINARY_NODE: {
		return visitDataNode(pNode, param);
	}
	case OP_NODE:
		if (pNode->m_iChildNum == 2) {
			return visitDyadicOpNode(OP_CODE(pNode), pNode, param);
		} else if (pNode->m_iChildNum == 1) {
			return visitUnaryOpNode(OP_CODE(pNode), pNode, param);
		} else {
			LOG(ERROR, "Operator node with %d operators is not supported!",
					pNode->m_iChildNum);
			throw new ParseException(
					"Operator node with %d operators is not supported!",
					pNode->m_iChildNum);
		}
	case FUNC_NODE: {
		assert(pNode->m_iChildNum == 1);
		return visitFuncNode(pNode->m_pszValue, pNode, param);
	}
	default:
		LOG(ERROR, "Unsupported ParseNode type %d", pNode->m_iType)
		;
		throw new ParseException("Unsupported ParseNode type %d",
				pNode->m_iType);
	}
}
