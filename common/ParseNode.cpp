#include "ParseNode.h"
#include <stdio.h>
#include <time.h>

extern const char* getTypeName(int type);

void buildPlanDefault(ParseNode* pNode) {
	for (auto pChild : pNode->m_children) {
		if (pChild && pChild->m_fnBuildPlan) {
			BUILD_PLAN(pChild);
		}
	};
}

void printTree(ParseNode* pRoot, int level) {
	int i;
	const char* pszTypeName;
	for (i = 0; i < level; ++i)
		printf("\t");
	if (pRoot == 0) {
		printf("NULL\n");
		return;
	}
	switch (pRoot->m_type) {
	case NodeType::OP:
	case NodeType::FUNC:
	case NodeType::INFO:
	case NodeType::DATATYPE:
		pszTypeName = getTypeName(pRoot->m_iValue);
		if (pszTypeName == 0)
			printf("%c\n", (char) pRoot->m_iValue);
		else
			printf("%s\n", pszTypeName);
		break;
	case NodeType::BINARY:
		printf("\\x");
		for (i = 0; i < pRoot->m_iValue; ++i) {
			printf("%02x", (unsigned char) pRoot->m_sValue[i]);
		}
		printf("\n");
		break;
	default:
		printf("%s\n", pRoot->m_sValue.c_str());
		break;
	}
	for (auto pChild : pRoot->m_children) {
		printTree(pChild, level + 1);
	}
}

ParseNode::ParseNode(ParseResult* p, NodeType type, int num) :
		m_type(type), m_fnBuildPlan(buildPlanDefault) {
	m_children.reserve(num);
	if (p != nullptr) {
		p->m_nodes.emplace_back(this);
	}
}

bool ParseNode::_collect(std::vector<ParseNode*>& result,
		const std::string sRemove) {
	if (m_type == NodeType::PARENT && m_sValue == sRemove) {
		for (auto pChild : m_children) {
			if (!pChild->_collect(result, sRemove)) {
				result.push_back(pChild);
			}
		}
		return true;
	} else {
		return false;
	}

}

void ParseNode::remove(const std::string sNewName, const std::string sRemove) {
	std::vector<ParseNode*> new_children;
	if (this->_collect(new_children, sRemove)) {
		m_children.swap(new_children);
		m_sValue = sNewName;
	}

}

static std::string trim_dup(ParseResult *p, int firstColumn, int lastColumn) {
	size_t j = 0;
	size_t iLen = lastColumn - firstColumn + 1;
	std::string result;
	for (size_t i = firstColumn; i <= lastColumn; ++i) {
		char c = p->m_sSql[i - 1];
		switch (c) {
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			continue;
		default:
			break;
		}
		result += c;
	}
	return result;
}

ParseNode* newFuncNode(ParseResult *p, const std::string& sName, int firstColumn,
		int lastColumn, int num, ...) {
	va_list va;
	ParseNode* pNode = new ParseNode(p, NodeType::FUNC, num);
	pNode->m_sExpr = trim_dup(p, firstColumn, lastColumn);
	pNode->m_sValue = sName;
	pNode->m_iValue = 0;
	va_start(va, num);
	for (size_t i = 0; i < num; ++i) {
		pNode->m_children.emplace_back(va_arg(va, ParseNode*));
	}
	va_end(va);
	return pNode;
}

ParseNode* newExprNode(ParseResult *p, int value, int firstColumn,
	int lastColumn, int num, ...) {
	va_list va;
	ParseNode* pNode = new ParseNode(p, NodeType::OP, num);
	pNode->m_sExpr = trim_dup(p, firstColumn, lastColumn);
	pNode->m_iValue = value;
	va_start(va, num);
	for (size_t i = 0; i < num; ++i) {
		pNode->m_children.emplace_back(va_arg(va, ParseNode*));
	}
	va_end(va);
	return pNode;
}

ParseNode* newIntNode(ParseResult *p, NodeType type, int value, int num, ...) {
	va_list va;
	ParseNode* pNode = new ParseNode(p, type, num);
	pNode->m_iValue = value;
	va_start(va, num);
	for (size_t i = 0; i < num; ++i) {
		pNode->m_children.emplace_back(va_arg(va, ParseNode*));
	}
	va_end(va);
	return pNode;
}

ParseNode* newParentNode(ParseResult *p, const std::string& sName, int num, ...) {
	assert(num > 0);
	va_list va;
	ParseNode* pNode = new ParseNode(p, NodeType::PARENT, num);
	pNode->m_sValue = sName;
	va_start(va, num);
	for (size_t i = 0; i < num; ++i) {
		pNode->m_children.emplace_back(va_arg(va, ParseNode*));
	}
	va_end(va);
	return pNode;
}

int64_t parseTime(const char* pszTime) {
	int iYear = 0;
	int iMonth = 0;
	int iDay = 0;
	int iHour = 0;
	int iMinute = 0;
	int iSecond = 0;

	struct tm time = {};
	int ret = sscanf(pszTime, "%4d-%2d-%2d %2d:%2d:%2d", &iYear, &iMonth, &iDay,
	&iHour, &iMinute, &iSecond);
	if (ret != 3 && ret != 6) {
		return 0;
	}

	time.tm_year = iYear - 1900;
	time.tm_mon = iMonth - 1;
	time.tm_mday = iDay;
	time.tm_hour = iHour;
	time.tm_min = iMinute;
	time.tm_sec = iSecond;

	time_t iRetTime = mktime(&time);
	if (iYear - 1900 != time.tm_year || iMonth - 1 != time.tm_mon
	|| iDay != time.tm_mday) {
		return 0;
	}
	return iRetTime * 1000000; // unit is microseconds
}




constexpr bool IS_DIGIT(char c) {
	return c >='0' && c<='9';
}
// pszSrc is quoted string, this function remove the quote and change \x to real value.
std::string cleanString(const char* pszSrc) {
	std::string result;
	for (size_t i = 1; ; ++i) {
		char c = pszSrc[i];
		if (c == pszSrc[0] || c =='\0') {
			//ignore end quote
			break;
		} else if (c == '\\') {
			if (IS_DIGIT(pszSrc[i + 1]) && IS_DIGIT(pszSrc[i + 2]) && IS_DIGIT(pszSrc[i + 3])) {
					c = (pszSrc[++i] - '0') * 64;
					c += (pszSrc[++i] - '0') * 8;
					c += pszSrc[++i] - '0';
			} else {
				c = pszSrc[++i];
				switch (c) {
				case 'n':
					c = '\n';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				default:
					break;
				}
			}
		}
		result += c;
	}
	return result;
}

std::string parseBinary(const char* pszSrc)
{
	std::string result;
	for (size_t i = 1; ; ++i) {
		char c = pszSrc[i];
		if (c == pszSrc[0] || c =='\0') {
			//ignore end quote
			break;
		}
		char szBuf[3];
		szBuf[0] = c;
		szBuf[1] = pszSrc[++i];
		szBuf[2] = 0;
		unsigned char a = strtol(szBuf,0, 16);
		result += (char)a;
	}
	return result;
}
