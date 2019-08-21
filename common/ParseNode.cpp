#include "ParseNode.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include <iomanip>

extern const char* getTypeName(int type);

void buildPlanDefault(const ParseNode* pNode) {
	for(size_t i=0;i<pNode->children();++i) {
		auto pChild = pNode->getChild(i);
		if (pChild && pChild->m_fnBuildPlan) {
			BUILD_PLAN(pChild);
		}
	};
}

void printTree(const ParseNode* pRoot, int level) {
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
	for(size_t i=0;i<pRoot->children();++i) {
		auto pChild = pRoot->getChild(i);
		printTree(pChild, level + 1);
	}
}

ParseNode::ParseNode(NodeType type,
		const std::string_view sExpr,
		size_t childNum,
		const ParseNode** children)
: m_type(type),m_sExpr(sExpr),
  m_fnBuildPlan(buildPlanDefault),
  m_iChildNum(childNum),
  m_children(children) {

}

ParseNode* newParseNode(ParseResult* p,
		NodeType type,
		const std::string_view sExpr,
		std::initializer_list<const ParseNode*> initList)
{
	std::vector<const ParseNode*> childrens = initList;
	return p->allocParseNodeFn(type, sExpr, childrens);
}

bool ParseNode::_collect(std::vector<const ParseNode*>& result,
		const std::string sRemove)const {
	if (m_type == NodeType::PARENT && m_sValue == sRemove) {
		for(size_t i=0;i<children();++i) {
			auto pChild = m_children[i];
			if (!pChild->_collect(result, sRemove)) {
				result.push_back(pChild);
			}
		}
		return true;
	} else {
		return false;
	}

}

ParseNode* ParseNode::merge(ParseResult* p, const std::string sNewName, const std::string sRemove) const{
	std::vector<const ParseNode*> new_children;

	if (this->_collect(new_children, sRemove)) {
		auto pResult = p->allocParseNodeFn(m_type, m_sExpr, new_children);
		pResult->m_sValue = sNewName;
		return pResult;
	}
	auto pResult = newParseNode(p, NodeType::PARENT, m_sExpr, {this});
	pResult->m_sValue = sNewName;
	return pResult;
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

