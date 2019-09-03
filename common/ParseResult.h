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

	std::string_view concate(std::initializer_list<const std::string_view> children) {
		size_t iLen = 0;
		for(auto& s: children) {
			iLen += s.size();
		}
		size_t iStart = 0;
		char* pAlloc = alloc(iLen);
		for(auto& s: children) {
			memcpy(pAlloc+ iStart,s.data(), s.size());
			iStart += s.size();
		}
		return std::string_view(pAlloc, iLen);
	}

	template <typename Iteratable>
	ParseNode* newParseNode(NodeType type,
			Operation op,
			const std::string_view sExpr,
			Iteratable children) {
		size_t iChildNum = children.size();
		size_t iChildrenAlloc = iChildNum * sizeof(ParseNode*);
		size_t iTotalAlloc = sizeof(ParseNode) + iChildrenAlloc;

		char* p = alloc(iTotalAlloc);
		if (p == nullptr) {
			PARSE_ERROR("Not enough parse buffer");
		}
		ParseNode** pChildren = new (p) ParseNode*[iChildNum];
		size_t index = 0;
		for(auto iter = children.begin(); iter != children.end(); ++iter) {
			pChildren[index++] = *iter;
		}
		p += iChildrenAlloc;
		return new(p) ParseNode(type, op, sExpr, iChildNum, pChildren);
	}


	ParseNode* newSimpleNode(NodeType type,
			int firstColumn,
			int lastColumn, Operation op = Operation::NONE) {
		std::initializer_list<ParseNode*> children;
		return newParseNode(type, op,
				getExpr(firstColumn, lastColumn), children);
	}



	ParseNode* newFuncNode(const std::string_view& sName, int firstColumn,
			int lastColumn,
			std::initializer_list<ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::FUNC,Operation::NONE, getExpr(firstColumn, lastColumn), children);
		pNode->m_sValue = sName;
		return pNode;
	}

	ParseNode* newExprNode(Operation op, int firstColumn,
		int lastColumn, std::initializer_list<ParseNode*> children) {
		return newParseNode(NodeType::OP, op,  getExpr(firstColumn, lastColumn), children);
	}

	ParseNode* newExprNode(Operation op, const std::string_view sExpr, std::initializer_list<ParseNode*> children) {
		return newParseNode(NodeType::OP, op,  sExpr, children);
	}

	ParseNode* newInfoNode(Operation op,
			int firstColumn, int lastColumn) {
		std::initializer_list<ParseNode*> children;
		return newParseNode(NodeType::INFO, op,  getExpr(firstColumn, lastColumn), children);
	}

	ParseNode* newParentNode(const std::string_view& sName,
			int firstColumn, int lastColumn,
			std::initializer_list<ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::PARENT,Operation::NONE, getExpr(firstColumn, lastColumn), children);
		pNode->m_sValue = sName;
		return pNode;
	}

	ParseNode* newPlanNode(const std::string_view& sName, Operation op,
				int firstColumn, int lastColumn,
			std::initializer_list<ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::PLAN,op, getExpr(firstColumn, lastColumn), children);
		pNode->m_sValue = sName;
		return pNode;
	}

	ParseNode* newListNode(const std::string_view& sName,
				int firstColumn, int lastColumn, std::initializer_list<ParseNode*> children) {
		ParseNode* pNode = newParseNode(NodeType::LIST,Operation::NONE, getExpr(firstColumn, lastColumn), children);
		pNode->m_sValue = sName;
		return pNode;
	}

	void mark() {m_iMark = m_iCurrent;}
	void restore() {m_iCurrent = m_iMark;}
private:
    std::string_view getExpr(int firstColumn, int lastColumn) {
    	return std::string_view(m_sSql.c_str() + firstColumn - 1,lastColumn - firstColumn + 1);
    }
	std::string m_sParseBuffer;
	size_t m_iCurrent = 0;
	size_t m_iMark = 0;
};

extern int parseInit(ParseResult* p);

extern int parseTerminate(ParseResult* p);

extern void parseSql(ParseResult *p, const std::string_view sql);


