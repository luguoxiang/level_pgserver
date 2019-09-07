#include "QueryRewritter.h"
#include "ParseException.h"

ParseNode* QueryRewritter::rewrite(ParseNode* pNode) {
	if(pNode == nullptr) {
		return nullptr;
	}
	switch (pNode->m_type) {
	case NodeType::LIST:
		pNode = rewriteList(pNode);
		break;
	case NodeType::OP:{
		bool hasOr = false;
		pNode = rewriteOp(pNode, hasOr);
		if(hasOr) {
			pNode = liftOrPredicate(pNode);
		}
		return pNode;
	}
	default:
		break;
	}
	for(size_t i=0;i<pNode->children();++i) {
		auto pChild = rewrite(pNode->getChild(i));
		pNode->setChild(i, pChild);
	}
	return pNode;
}

ParseNode* QueryRewritter::rewriteOp(ParseNode* pNode, bool& hasOr) {
	assert(pNode->m_type == NodeType::OP);
	switch (pNode->getOp()) {
	case Operation::AND:
	case Operation::OR:
		for(size_t i=0;i<pNode->children();++i) {
			auto pChild = pNode->getChild(i);
			bool childHasOr = false;
			pNode->setChild(i, rewriteOp(pChild, childHasOr));
			hasOr = hasOr || childHasOr;
		}
		return pNode;
	case Operation::IN:
		pNode = rewriteInOrNotIN(pNode, true);
		hasOr = pNode->getOp() == Operation::OR;
		break;
	case Operation::NOT_IN:
		pNode = rewriteInOrNotIN(pNode, false);
		hasOr = pNode->getOp() == Operation::OR;
		break;
	default:
		break;
	}
	return pNode;
}
void QueryRewritter::collectElements(ParseNode* pNode,
		const std::string_view sName, std::vector<ParseNode*>& elements) {
	assert(pNode);
	if (pNode->m_type == NodeType::LIST && pNode->getString() == sName) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			collectElements(pNode->getChild(i), sName, elements);
		}
	} else {
		elements.push_back(pNode);
	}
}

ParseNode* QueryRewritter::rewriteList(ParseNode* pParent) {
	assert(pParent->m_type == NodeType::LIST);

	std::vector<ParseNode*> elements;
	auto sName = pParent->getString();
	collectElements(pParent, sName, elements);

	pParent = m_result.newParseNode(NodeType::LIST,
			Operation::NONE, pParent->m_sExpr, elements);
	pParent->setString(sName);
	return pParent;
}


ParseNode* QueryRewritter::liftOrPredicate(ParseNode* pParent) {
	assert(pParent != nullptr && pParent->m_type == NodeType::OP);
	assert(pParent->getOp() == Operation::AND || pParent->getOp() == Operation::OR);

	std::vector<ParseNode*> operators;
	collectOrOperators(pParent, operators);

	assert(operators.size() > 1);
	return m_result.newParseNode(NodeType::OP, Operation::OR, pParent->m_sExpr, operators);
}

ParseNode* QueryRewritter::rewriteInOrNotIN(ParseNode* pNode, bool in) {

	assert(pNode);
	auto pLeft = pNode->getChild(0);
	auto pValue = rewriteList(pNode->getChild(1));
	assert(pLeft && pValue);

	std::vector<ParseNode*> expressions(pValue->children());

	for (size_t i = 0; i < pValue->children(); ++i) {
		auto pChild = pValue->getChild(i);
		if (!pChild->isConst()) {
			PARSE_ERROR("expect const value:", pChild->m_sExpr);
		}
		Operation op = in ? Operation::COMP_EQ: Operation::COMP_NE;

		auto sExpr = m_result.concate({pLeft->m_sExpr,in ? "=" : "!=", pChild->m_sExpr});

		pChild = m_result.newExprNode(op, sExpr, { pLeft, pChild });
		expressions[i] = pChild;
	}

	assert(!expressions.empty());
	if(expressions.size() == 1) {
		return expressions[0];
	}

	return m_result.newParseNode(NodeType::OP, Operation::OR,
			pNode->m_sExpr, expressions);
}

bool QueryRewritter::hasOrPredicate(const ParseNode* pNode) {
	if(pNode->getOp() == Operation::OR) {
		return true;
	}
	if(pNode->getOp() == Operation::IN) {
		assert(pNode->children() == 2);
		return true;
	}
	return pNode->anyChildOf([this](size_t index, auto pChild) {
		return hasOrPredicate(pChild);
	});

	return false;
}

void QueryRewritter::collectOrOperators(ParseNode* pPredicate,
		std::vector<ParseNode*>& operators) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}
	switch (pPredicate->getOp()) {
	case Operation::AND: {
		if(!hasOrPredicate(pPredicate)) {
			operators.push_back(pPredicate);
			break;
		}
		assert(pPredicate->children() == 2);
		std::vector<ParseNode*> left, right;

		collectOrOperators(pPredicate->getChild(0), left);
		collectOrOperators(pPredicate->getChild(1), right);

		for (size_t i = 0; i < left.size(); ++i) {
			for (size_t j = 0; j < right.size(); ++j) {
				auto pOp = m_result.newExprNode(Operation::AND, "", {
						left[i], right[j] });
				operators.push_back(pOp);
			}
		}
		break;
	}
	case Operation::OR: {
		for (size_t i = 0; i < pPredicate->children(); ++i) {
			collectOrOperators(pPredicate->getChild(i), operators);
		}
		break;
	}
	default:
		operators.push_back(pPredicate);
		break;
	}
}
