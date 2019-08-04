#include "execution/ProjectionPlan.h"
#include "execution/ParseTools.h"
#include "common/ParseException.h"

bool ProjectionPlan::project(ParseNode* pNode, const char* pszName)
{
	ProjectionInfo info;
	int iSubIndex = m_pPlan->addProjection(pNode);
	if (iSubIndex < 0)
	{
		return false;
	}
	info.m_iSubIndex = iSubIndex;
	info.m_pszName = pszName;
	info.m_pszRaw = pNode->m_pszExpr;
	m_map[pszName] = m_proj.size();
	m_proj.push_back(info);
	return true;
}
