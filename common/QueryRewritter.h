#pragma once

#include <vector>
#include "ParseResult.h"

class QueryRewritter {
public:
	QueryRewritter(ParseResult& result):m_result(result) {}
	ParseNode* rewrite(ParseNode* pRoot);
private:
	ParseNode* rewriteList(ParseNode* pRoot);
	ParseNode* liftOrPredicate(ParseNode* pRoot);
	bool hasOrPredicate(ParseNode* pNode);
	ParseNode* rewriteInOrNotIN(ParseNode* pNode, bool in);
	void collectElements(ParseNode* pRoot, const std::string_view sName, std::vector<ParseNode*>& elements);
	void collectOrOperators(ParseNode* pPredicate, std::vector<ParseNode*>& operators);
	ParseResult& m_result;
};
