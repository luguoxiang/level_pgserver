#include "execution/ProjectionPlan.h"
#include "common/ParseException.h"
#include "execution/DBDataTypeHandler.h"
bool ProjectionPlan::project(const ParseNode* pNode, const std::string_view& sName) {
	ProjectionInfo info;

	if(!pNode->isConst()) {
		int iSubIndex = m_pPlan->addProjection(pNode);
		if (iSubIndex < 0) {
			return false;
		}
		info.m_iSubIndex = iSubIndex;

	} else if(pNode->m_type == NodeType::PARAM) {
		return false;
	} else {
		info.m_iSubIndex = -1;
	}

	m_map[sName] = m_proj.size();
	info.m_sName = sName;
	info.m_sRaw = pNode->m_sExpr;
	info.m_pNode = pNode;
	m_proj.push_back(info);
	return true;
}

DBDataType ProjectionPlan::getResultType(size_t index) {
	auto& proj = m_proj[index];
	if( proj.m_iSubIndex < 0) {
		return proj.m_pNode->getConstResultType();
	}
	assert(index < m_proj.size());
	size_t iSubIndex = proj.m_iSubIndex;
	auto type =  m_pPlan->getResultType(iSubIndex);
	assert(type != DBDataType::UNKNOWN);
	return type;
}

void ProjectionPlan::getResult(size_t index, ExecutionResult& result, DBDataType type) {
	auto& proj = m_proj[index];
	if( proj.m_iSubIndex < 0) {
		DBDataTypeHandler::getHandler(type)->fromNode(proj.m_pNode, result);
		return;
	}
	assert(index < m_proj.size());
	size_t iSubIndex =proj.m_iSubIndex;
	return m_pPlan->getResult(iSubIndex, result, type);
}
