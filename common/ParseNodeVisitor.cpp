#include "ParseNodeVisitor.h"

void ParentFlattenVisitor::visit(const ParseNode* pNode) {
	if(pNode->m_type != NodeType::PARENT) {
		m_fn(pNode);
		return;
	}
	for(size_t i=0;i<pNode->children();++i) {
		visit(pNode->getChild(i));
	}
}

void ExprFlattenVisitor::visit(const ParseNode* pNode) {
	if(pNode->m_type != NodeType::OP || pNode->m_op != m_op) {
		m_fn(pNode);
		return;
	}
	for(size_t i=0;i<pNode->children();++i) {
		visit(pNode->getChild(i));
	}
}



