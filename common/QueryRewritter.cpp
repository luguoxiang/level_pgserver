#include "QueryRewritter.h"
#include "ParseException.h"

ParseNode* QueryRewritter::rewrite(ParseNode* pNode) {
	if(pNode == nullptr) {
		return nullptr;
	}
	switch (pNode->m_type) {
	case NodeType::LIST:
		return rewriteList(pNode);
	case NodeType::OP:
		switch (pNode->getOp()) {
		case Operation::AND:
			if(hasOrPredicate(pNode)) {
				return liftOrPredicate(pNode);
			} else {
				return rewriteAnd(pNode);
			}
		case Operation::OR:
			return liftOrPredicate(pNode);
		default:
			return rewriteSimpleExpr(pNode);
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

void QueryRewritter::collectElements(ParseNode* pNode,
		const std::string_view sName, std::vector<ParseNode*>& elements) {
	assert(pNode);
	if (pNode->m_type == NodeType::LIST && pNode->m_sValue == sName) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			collectElements(pNode->getChild(i), sName, elements);
		}
	} else {
		pNode = rewrite(pNode);
		elements.push_back(pNode);
	}
}

ParseNode* QueryRewritter::rewriteList(ParseNode* pParent) {
	assert(pParent->m_type == NodeType::LIST);

	std::vector<ParseNode*> elements;
	auto sName = pParent->m_sValue;
	collectElements(pParent, sName, elements);

	pParent = m_result.newParseNode(NodeType::LIST,
			Operation::NONE, pParent->m_sExpr, elements);
	pParent->m_sValue = sName;
	return pParent;
}

ParseNode* QueryRewritter::rewriteAnd(ParseNode* pParent) {
	assert(pParent != nullptr && pParent->m_type == NodeType::OP);
	assert(pParent->getOp() == Operation::AND && pParent->children() == 2);

	for(size_t i=0;i<pParent->children();++i) {
		auto pChild = rewriteSimpleExpr(pParent->getChild(i));
		if(pChild->getOp() == Operation::AND) {
			pChild = rewriteAnd(pChild);
		} else {
			pChild = rewriteSimpleExpr(pChild);
		}

		pParent->setChild(i, pChild);
	}

	return pParent;
}

ParseNode* QueryRewritter::rewriteSimpleExpr(ParseNode* pNode) {
	assert(pNode!= nullptr);
	if (pNode->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pNode->m_sExpr);
	}

	assert(pNode->getOp() != Operation::AND && pNode->getOp() != Operation::OR);

	switch (pNode->getOp()) {
	case Operation::IN:
		return rewriteInOrNotIN(pNode, true);
	case Operation::NOT_IN:
		return rewriteInOrNotIN(pNode, false);
	default:
		return pNode;
	}
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
	auto pValue = rewrite(pNode->getChild(1));
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

	return m_result.newParseNode(NodeType::OP, Operation::AND,
			pNode->m_sExpr, expressions);
}

bool QueryRewritter::hasOrPredicate(ParseNode* pNode) {
	if(pNode->getOp() == Operation::OR) {
		return true;
	}
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		if(hasOrPredicate(pChild)) {
			return true;
		}
	}
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
			pPredicate = rewriteAnd(pPredicate);
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
		pPredicate = rewriteSimpleExpr(pPredicate);
		operators.push_back(pPredicate);
		break;
	}
}
