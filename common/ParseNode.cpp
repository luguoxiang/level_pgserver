#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>


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
	case NodeType::BINARY: {
		std::cout << "\\x";
		auto sValue = pRoot->getString();
		for (i = 0; i < sValue.size(); ++i) {
			std::cout << std::hex << sValue[i];
		}
		std::cout << std::endl;
		break;
	}
	case NodeType::PARENT:
	case NodeType::NAME:
	case NodeType::PLAN:
	case NodeType::LIST:
		std::cout << pRoot->getString() << std::endl;
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
		ParseNode** children) :
				m_type(type),
				m_sExpr(sExpr),
				m_iChildNum(childNum),
				m_children(children), m_op(op) {

}




int64_t parseTime(const char* pszTime) {
	int iYear = 0;
	int iMonth = 0;
	int iDay = 0;
	int iHour = 0;
	int iMinute = 0;
	int iSecond = 0;

	struct tm time;
	int ret = sscanf(pszTime, "%4d-%2d-%2d %2d:%2d:%2d",
			&iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
	if (ret != 3 && ret != 6) {
		return 0;
	}

	memset(&time, 0, sizeof(struct tm));
	time.tm_year = iYear - 1900;
	time.tm_mon = iMonth - 1;
	time.tm_mday = iDay;
	time.tm_hour = iHour;
	time.tm_min = iMinute;
	time.tm_sec = iSecond;

	time_t iRetTime = timegm(&time);
	if (iYear - 1900 != time.tm_year
			|| iMonth - 1 != time.tm_mon
			|| iDay != time.tm_mday) {
		return 0;
	}
	return iRetTime; // unit is seconds
}
