#include "QueryRewritter.h"
#include "ParseException.h"

ParseNode* QueryRewritter::rewrite(ParseNode* pNode) {
	if(pNode == nullptr) {
		return nullptr;
	}
	switch (pNode->m_type) {
	case NodeType::LIST: {
		std::vector<ParseNode*> elements;
		auto sName = pNode->m_sValue;
		collectElements(pNode, sName, elements);
		pNode = m_result.newParseNode(NodeType::LIST,
				Operation::NONE, pNode->m_sExpr, elements);
		pNode->m_sValue = sName;
		break;
	}
	case NodeType::OP:
		switch (pNode->m_op) {
		case Operation::AND:
		case Operation::OR: {
			std::vector<ParseNode*> operators;
			collectOrOperators(pNode, operators);
			if (operators.size() == 1) {
				return operators[0];
			}
			return m_result.newParseNode(NodeType::OP, Operation::OR,
					pNode->m_sExpr, operators);
		}
		case Operation::IN:
			return rewriteInOrNotIN(pNode, true);
		case Operation::NOT_IN:
			return rewriteInOrNotIN(pNode, false);
		default:
			break;
		}
	default:
		break;
	}
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChild(i);
		pNode->setChild(i, rewrite(pChild));
	}
	return pNode;
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

void QueryRewritter::collectElements(ParseNode* pNode,
		const std::string_view sName, std::vector<ParseNode*>& elements) {
	assert(pNode);
	if (pNode->m_type == NodeType::LIST && pNode->m_sValue == sName) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			collectElements(pNode->getChild(i), sName, elements);
		}
	} else {
		elements.push_back(pNode);
	}
}

void QueryRewritter::collectOrOperators(ParseNode* pPredicate,
		std::vector<ParseNode*>& operators) {
	assert(pPredicate);
	if (pPredicate->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}
	switch (pPredicate->m_op) {
	case Operation::AND: {
		assert(pPredicate->children() == 2);
		std::vector<ParseNode*> left, right;

		collectOrOperators(pPredicate->getChild(0), left);
		collectOrOperators(pPredicate->getChild(1), right);

		if (left.size() == 1 && right.size() == 1) {
			operators.push_back(pPredicate);
		} else {
			for (size_t i = 0; i < left.size(); ++i) {
				for (size_t j = 0; j < right.size(); ++j) {
					auto pOp = m_result.newExprNode(Operation::AND, "", {
							left[i], right[j] });
					operators.push_back(pOp);
				}
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
		pPredicate = rewrite(pPredicate);
		operators.push_back(pPredicate);
		break;
	}
}
