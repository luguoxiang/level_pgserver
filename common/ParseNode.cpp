#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>

void buildPlanDefault(const ParseNode* pNode) {
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		if (pChild && pChild->m_fnBuildPlan) {
			BUILD_PLAN(pChild);
		}
	};
}

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
	case NodeType::BINARY:
		std::cout << "\\x";
		for (i = 0; i < pRoot->m_iValue; ++i) {
			std::cout << std::hex << pRoot->m_sValue[i];
		}
		std::cout << std::endl;
		break;
	case NodeType::PARENT:
	case NodeType::NAME:
		std::cout << pRoot->m_sValue << std::endl;
		break;
	default:
		std::cout << pRoot->m_sExpr << std::endl;
		break;
	}
	for (size_t i = 0; i < pRoot->children(); ++i) {
		auto pChild = pRoot->getChild(i);
		printTree(pChild, level + 1);
	}
}

ParseNode::ParseNode(NodeType type,
		Operation op,
		const std::string_view sExpr,
		size_t childNum,
		const ParseNode** children) :
				m_type(type),
				m_sExpr(sExpr),
				m_fnBuildPlan(buildPlanDefault),
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

