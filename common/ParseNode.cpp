#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>

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
		std::cout<<"\t";
	if (pRoot == 0) {
		std::cout<<"NULL"<<std::endl;
		return;
	}
	switch (pRoot->m_type) {
	case NodeType::OP:
	case NodeType::FUNC:
	case NodeType::INFO:
	case NodeType::DATATYPE:
		pszTypeName = getTypeName(pRoot->m_iValue);
		if (pszTypeName == 0){
			std::cout<<(char) pRoot->m_iValue <<std::endl;
		}else {
			std::cout<< pszTypeName <<std::endl;
		}
		break;
	case NodeType::BINARY:
		std::cout <<"\\x";
		for (i = 0; i < pRoot->m_iValue; ++i) {
			std::cout << std::hex << pRoot->m_sValue[i];
		}
		std::cout <<std::endl;
		break;
	case NodeType::PARENT:
	case NodeType::NAME:
		std::cout<<pRoot->m_sValue<<std::endl;
		break;
	default:
		std::cout<<pRoot->m_sExpr<<std::endl;
		break;
	}
	for (auto pChild : pRoot->m_children) {
		printTree(pChild, level + 1);
	}
}

ParseNode::ParseNode(ParseResult* p,
		NodeType type,
		int firstColumn,
		int lastColumn,
		std::initializer_list<ParseNode*> children) : m_type(type),
			m_fnBuildPlan(buildPlanDefault),
			m_sExpr(p->m_sSql.c_str() + firstColumn - 1,lastColumn - firstColumn + 1 ){
	m_children.reserve(children.size());
	if (p != nullptr) {
		p->m_nodes.emplace_back(this);
	}
	for(auto pChild : children) {
		m_children.emplace_back(pChild);
	}
}

ParseNode::ParseNode(ParseResult* p,
   		NodeType type,
		const std::string_view sExpr,
		std::initializer_list<ParseNode*> children) : m_type(type),
			m_fnBuildPlan(buildPlanDefault),
			m_sExpr(sExpr) {
	m_children.reserve(children.size());
	if (p != nullptr) {
		p->m_nodes.emplace_back(this);
	}
	for(auto pChild : children) {
		m_children.emplace_back(pChild);
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

ParseNode* ParseNode::merge(ParseResult* p, const std::string sNewName, const std::string sRemove) {
	std::vector<ParseNode*> new_children;
	if (this->_collect(new_children, sRemove)) {
		m_children.swap(new_children);
		m_sValue = sNewName;
		return this;
	}
	ParseNode* pNode = new ParseNode(p, NodeType::PARENT, m_sExpr, {this});
	pNode->m_sValue = sNewName;
	return pNode;
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




int64_t parseTime(std::string_view sTime) {
	std::tm time;
	std::stringstream ss;
	ss << sTime;
	ss >> std::get_time(&time, "%Y-%m-%d %H:%M:%S");

	time_t iRetTime = mktime(&time);
	if (iRetTime == -1) {
		return 0;
	}
	return iRetTime; // unit is seconds
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
