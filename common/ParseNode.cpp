#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <absl/strings/escaping.h>


char const* getOperationName(Operation op) {
	switch(op) {
	case Operation::COMP_EQ:
		return "=";
	case Operation::COMP_NE:
		return "!=";
	case Operation::COMP_LE:
		return "<=";
	case Operation::COMP_LT:
		return "<";
	case Operation::COMP_GT:
		return ">";
	case Operation::COMP_GE:
		return ">=";
	case Operation::COMP_LIKE:
		return "like";
	case Operation::COMP_IN:
		return "in";
	case Operation::COMP_NOT_IN:
		return "not in";
	case Operation::MINUS:
		return "-";
	case Operation::ADD:
		return "+";
	case Operation::SUB:
		return "-";
	case Operation::DIV:
		return "/";
	case Operation::MUL:
		return "*";
	case Operation::MOD:
		return "%";
	case Operation::MEMBER:
		return ".";
	case Operation::AS:
		return "as";
	case Operation::AND:
		return "and";
	case Operation::OR:
		return "or";
	case Operation::ASC:
		return "asc";
	case Operation::DESC:
		return "desc";
	case Operation::ALL_COLUMNS:
		return "*";
	case Operation::SHOW_TABLES:
		return "show tables";
	case Operation::DESC_TABLE:
		return "desc";
	case Operation::SELECT:
		return "select";
	case Operation::INSERT:
		return "insert";
	case Operation::REMOVE:
		return "remove";
	case Operation::EXPLAIN:
		return "explain";
	case Operation::VALUES:
		return "values";
	case Operation::SELECT_WITH_SUBQUERY:
		return "select()";
	case Operation::TEXT_PARAM:
		return "text_param";
	case Operation::BINARY_PARAM:
		return "binary_param";
	case Operation::UNBOUND_PARAM:
		return "unbound_param";
	case Operation::NONE:
		return "none";
	default:
		return "unknown";
	}
}

ParseNode::ParseNode(NodeType type,
		Operation op,
		const std::string_view sExpr,
		size_t childNum,
		ParseNode** children) :
				m_type(type),
				m_sExpr(sExpr),
				m_iChildNum(childNum),
				m_children(children), m_op(op) {

}
void ParseNode::printHead(size_t level) {
	for (int i = 0; i < level; ++i) {
		if (i < level - 1 ) {
			std::cout<< "    ";
		}else {
			std::cout << "|-- ";
		}
	}
}
void ParseNode::print(size_t level) const {
	printHead(level);
	switch (m_type) {
	case NodeType::OP:
	case NodeType::INFO:
		std::cout << getOperationName(m_op) << std::endl;
		break;
	case NodeType::PARENT:
	case NodeType::NAME:
	case NodeType::PLAN:
	case NodeType::LIST:
		std::cout << m_sValue << std::endl;
		break;
	case NodeType::BINARY:
	case NodeType::PARAM:
		std::cout << absl::BytesToHexString(m_sValue) << std::endl;
		break;
	default:
		std::cout << m_sExpr << std::endl;
		break;
	}
	forEachChild([level](size_t index, auto pChild) {
		if(pChild == nullptr) {
			printHead(level + 1);
			std::cout << "null" << std::endl;
		} else {
			pChild->print(level + 1);
		}
	});

}

int64_t parseTime(std::string_view sTime) {
	std::tm time = {};
	std::stringstream ss;
	ss << sTime;
	if (sTime.length() <19) {
		ss >> std::get_time(&time, "%Y-%m-%d");
	} else {
		ss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");
	}
#ifdef _WIN32
	time_t iRetTime = _mkgmtime(&time);
#else
	time_t iRetTime = timegm(&time);
#endif
	if (iRetTime == -1) {
		return 0;
	}
	return iRetTime; // unit is seconds
}
