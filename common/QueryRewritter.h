#pragma once

#include <vector>
#include "ParseResult.h"

class QueryRewritter {
public:
	QueryRewritter(ParseResult& result):m_result(result) {}
	ParseNode* rewrite(ParseNode* pRoot);
private:
	void collectElements(ParseNode* pRoot, std::vector<ParseNode*>& elements);
	void collectOrOperators(ParseNode* pPredicate, std::vector<ParseNode*>& operators);
	ParseResult& m_result;
};
