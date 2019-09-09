#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <absl/strings/escaping.h>

void printTree(const ParseNode* pRoot, int level) {
	int i;
	for (i = 0; i < level; ++i)
		std::cout << "\t";
	if (pRoot == 0) {
		std::cout << "NULL" << std::endl;
		return;
	}
	switch (pRoot->m_type) {
	case NodeType::OP:
	case NodeType::INFO:
		std::cout << pRoot->m_sExpr << std::endl;
		break;
	case NodeType::PARENT:
	case NodeType::NAME:
	case NodeType::PLAN:
	case NodeType::LIST:
		std::cout << pRoot->getString() << std::endl;
		break;
	case NodeType::BINARY:
	case NodeType::PARAM:
		std::cout << absl::BytesToHexString(pRoot->getString()) << std::endl;
		break;
	default:
		std::cout << pRoot->m_sExpr << std::endl;
		break;
	}
	pRoot->forEachChild([level](size_t index, auto pChild) {
		printTree(pChild, level + 1);
	});

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


int64_t parseTime(std::string_view sTime) {
	std::tm time = {};
	std::stringstream ss;
	ss << sTime;
	if (sTime.length() < 12) {
		ss >> std::get_time(&time, "%Y-%m-%d");
	} else {
		ss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");
	}
	time_t iRetTime = timegm(&time);
	if (iRetTime == -1) {
		return 0;
	}
	return iRetTime; // unit is seconds
}
