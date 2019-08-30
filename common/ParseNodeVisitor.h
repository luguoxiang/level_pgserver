#pragma once
#include "common/ParseNode.h"
#include <functional>
using VisitNodeFn = std::function<void (const ParseNode* pNode)>;

class ParentFlattenVisitor {
public:

	ParentFlattenVisitor(VisitNodeFn fn) : m_fn(fn) {}
	void visit(const ParseNode* pNode);
private:
	VisitNodeFn m_fn;
};

class ExprFlattenVisitor {
public:

	ExprFlattenVisitor(Operation op,VisitNodeFn fn) : m_fn(fn), m_op(op) {}
	void visit(const ParseNode* pNode);
private:
	VisitNodeFn m_fn;
	Operation m_op;
};
