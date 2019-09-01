#include "QueryRewritter.h"
#include "ParseException.h"

ParseNode* QueryRewritter::rewrite(ParseNode* pNode) {
	switch (pNode->m_type) {
	case NodeType::LIST: {
		std::vector<ParseNode*> elements;
		collectElements(pNode, elements);
		pNode = m_result.newParseNode(NodeType::LIST, Operation::NONE,
				pNode->m_sExpr, elements);
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
		case Operation::IN: {
			auto pLeft = pNode->getChildNonConst(0);
			auto pValue = pNode->getChildNonConst(1);
			std::vector<ParseNode*> expressions(pValue->children());

			for (size_t i = 0; i < pValue->children(); ++i) {
				auto pChild = pValue->getChildNonConst(i);
				if(!pChild->isConst()) {
					PARSE_ERROR("expect const value:", pChild->m_sExpr);
				}

				pChild = m_result.newExprNode(Operation::COMP_EQ,pChild->m_sExpr, {pLeft, pChild});
				expressions.push_back(pChild);
			}
			return m_result.newParseNode(NodeType::OP, Operation::AND,
					pNode->m_sExpr, expressions);
		}
		case Operation::NOT_IN:{
			auto pLeft = pNode->getChildNonConst(0);
			auto pValue = pNode->getChildNonConst(1);
			std::vector<ParseNode*> expressions(pValue->children());

			for (size_t i = 0; i < pValue->children(); ++i) {
				auto pChild = pValue->getChildNonConst(i);
				if(!pChild->isConst()) {
					PARSE_ERROR("expect const value:", pChild->m_sExpr);
				}

				pChild = m_result.newExprNode(Operation::COMP_NE,pChild->m_sExpr, {pLeft, pChild});
				expressions.push_back(pChild);
			}
			return m_result.newParseNode(NodeType::OP, Operation::AND,
					pNode->m_sExpr, expressions);
		}
		default:
			break;
		}
	default:
		break;
	}
	for (size_t i = 0; i < pNode->children(); ++i) {
		auto pChild = pNode->getChildNonConst(i);
		pNode->setChild(i, rewrite(pChild));
	}
	return pNode;
}

void QueryRewritter::collectElements(ParseNode* pNode, std::vector<ParseNode*>& elements) {
	if (pNode->m_type == NodeType::LIST) {
		for (size_t i = 0; i < pNode->children(); ++i) {
			collectElements(pNode->getChildNonConst(i), elements);
		}
	} else {
		elements.push_back(pNode);
	}
}

void QueryRewritter::collectOrOperators(ParseNode* pPredicate,
		std::vector<ParseNode*>& operators) {
	if (pPredicate->m_type != NodeType::OP) {
		PARSE_ERROR("Unsupported predicate ", pPredicate->m_sExpr);
	}
	switch (pPredicate->m_op) {
	case Operation::AND: {
		assert(pPredicate->children() == 2);
		std::vector<ParseNode*> left, right;

		collectOrOperators(pPredicate->getChildNonConst(0), left);
		collectOrOperators(pPredicate->getChildNonConst(1), right);

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
			collectOrOperators(pPredicate->getChildNonConst(i), operators);
		}
		break;
	}
	default:
		pPredicate = rewrite(pPredicate);
		operators.push_back(pPredicate);
		break;
	}
}
