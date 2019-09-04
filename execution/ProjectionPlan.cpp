#include "execution/ProjectionPlan.h"
#include "common/ParseException.h"

bool ProjectionPlan::project(const ParseNode* pNode, const std::string_view& sName) {
	ProjectionInfo info;
	int iSubIndex = m_pPlan->addProjection(pNode);
	if (iSubIndex < 0) {
		return false;
	}

	info.m_iSubIndex = iSubIndex;
	info.m_sName = sName;
	info.m_sRaw = pNode->m_sExpr;
	info.pNode = pNode;
	m_map[sName] = m_proj.size();
	m_proj.push_back(info);
	return true;
}
