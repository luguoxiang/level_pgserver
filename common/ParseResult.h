#pragma once

#include "ParseNode.h"
#include "ParseException.h"





class ParseResult {
public:
	ParseResult();

	std::string m_sSql;
	void* m_scanInfo = nullptr;
	ParseNode* m_pResult = nullptr;
	std::string m_sError;
	int m_iStartCol = 0;
	int m_iEndCol = 0;
	int m_iLine = 0;
	int m_yycolumn = 0;
	int m_yylineno = 0;

	std::vector<ParseNode*> m_bindParamNodes;


	void initParse(const std::string_view sql);
	char* alloc(size_t size);

	ParseNode* merge(const ParseNode* pNode, const std::string sNewName, const std::string sRemove) ;

	template <typename Iterator>
	ParseNode* newParseNode(NodeType type,
			const std::string_view sExpr,
			size_t iChildNum, Iterator begin, Iterator end) {
		size_t iChildrenAlloc = iChildNum * sizeof(ParseNode*);
		size_t iTotalAlloc = sizeof(ParseNode) + iChildrenAlloc;

		char* p = alloc(iTotalAlloc);
		if (p == nullptr) {
			PARSE_ERROR("Not enough parse buffer");
		}
		const ParseNode** pChildren = new (p) const ParseNode*[iChildNum];
		size_t index = 0;
		for(Iterator iter = begin; iter != end; ++iter) {
			pChildren[index++] = *iter;
		}
		p += iChildrenAlloc;
		return new(p) ParseNode(type, sExpr, iChildNum, pChildren);
	}

	ParseNode* newParseNode(NodeType type,
			const std::string_view sExpr,
			std::initializer_list<const ParseNode*> children = {})  {
		return newParseNode(type, sExpr, children.size(), children.begin(), children.end())  ;
	}

	ParseNode* newParseNode(NodeType type,
			int firstColumn,
			int lastColumn,
			std::initializer_list<const ParseNode*> children = {}) {
		std::string_view sExpr(m_sSql.c_str() + firstColumn - 1,lastColumn - firstColumn + 1);
		return newParseNode(type, sExpr,  children.size(), children.begin(), children.end());
	}

	ParseNode* newFuncNode(const std::string_view& sName, int firstColumn,
			int lastColumn,
			std::initializer_list<const ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::FUNC,firstColumn, lastColumn, children);
		pNode->m_sValue = sName;
		return pNode;
	}

	ParseNode* newExprNode(Operation op, int firstColumn,
		int lastColumn, std::initializer_list<const ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::OP,firstColumn, lastColumn, children);
		pNode->m_op = op;
		return pNode;
	}

	ParseNode* newInfoNode(Operation op,
			int firstColumn, int lastColumn) {
		ParseNode* pNode = newParseNode(NodeType::INFO,  firstColumn, lastColumn, {});
		pNode->m_op = op;
		return pNode;
	}

	ParseNode* newParentNode(const std::string_view& sName,
			int firstColumn, int lastColumn,
			std::initializer_list<const ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::PARENT,  firstColumn, lastColumn, children);
		pNode->m_sValue = sName;
		return pNode;
	}

	void mark() {m_iMark = m_iCurrent;}
	void restore() {m_iCurrent = m_iMark;}
private:
    bool collect(const ParseNode* pNode, std::vector<const ParseNode*>& children, const std::string sRemove) ;

	std::string m_sParseBuffer;
	size_t m_iCurrent = 0;
	size_t m_iMark = 0;
};

extern int parseInit(ParseResult* p);

extern int parseTerminate(ParseResult* p);

extern void parseSql(ParseResult *p, const std::string_view sql);


