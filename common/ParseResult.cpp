#include "ParseResult.h"

constexpr int PARSE_BUFFER_SIZE = 512 * 1024;

ParseResult::ParseResult() : m_sParseBuffer(PARSE_BUFFER_SIZE, ' ') {
}

char* ParseResult::alloc(size_t size)  {
	if (m_iCurrent + size > PARSE_BUFFER_SIZE) {
		return nullptr;
	}
	char* alloc = m_sParseBuffer.data() + m_iCurrent;
	m_iCurrent += size;
	return alloc;
}

void ParseResult::initParse(const std::string_view sql) {
	m_yycolumn = 1;
	m_yylineno = 1;
	m_iCurrent = 0;
	m_pResult = nullptr;
	m_bindParamNodes.clear();
	m_sSql.assign(sql.data(), sql.length());
	m_sError = "";
}

bool ParseResult::collect(const ParseNode* pNode, std::vector<const ParseNode*>& result, const std::string sRemove) {
		if (pNode->m_type == NodeType::PARENT && pNode->m_sValue == sRemove) {
			for(size_t i=0;i<pNode->children();++i) {
				auto pChild = pNode->getChild(i);
				if (!collect(pChild, result, sRemove)) {
					result.push_back(pChild);
				}
			}
			return true;
		} else {
			return false;
		}

	}

ParseNode* ParseResult::merge(const ParseNode* pNode, const std::string sNewName, const std::string sRemove) {
	std::vector<const ParseNode*> new_children;

	if (collect(pNode, new_children, sRemove)) {
		auto pResult = newParseNode(pNode->m_type, Operation::NONE, pNode->m_sExpr, new_children);
		pResult->m_sValue = sNewName;
		return pResult;
	}
	std::initializer_list<const ParseNode*> children = {pNode};

	auto pResult = newParseNode(NodeType::PARENT, Operation::NONE, pNode->m_sExpr, children);

	pResult->m_sValue = sNewName;
	return pResult;
}

