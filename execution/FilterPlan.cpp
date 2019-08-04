#include "execution/FilterPlan.h"
#include "common/ParseException.h"

namespace
{
bool doLike(const char* a, size_t la, const char* b, size_t lb)
{

	if (la < lb)
		return false;
	for (size_t i = 0; i <= la - lb; ++i)
	{
		size_t j = 0;
		for (j = 0; j < lb; ++j)
		{
			if (a[i + j] != b[j])
				break;
		}
		if (j == lb)
			return true;
	}
	return false;
}

bool checkFilter(int iOpCode, int n)
{
	switch (iOpCode)
	{
	case COMP_EQ:
		return n == 0;
	case COMP_NE:
		return n != 0;
	case COMP_LE:
		return n <= 0;
	case COMP_LT:
		return n < 0;
	case COMP_GT:
		return n > 0;
	case COMP_GE:
		return n >= 0;
	default:
		PARSE_ERROR("Unsupported operation %d", iOpCode);
		return 0;
	}
}
}
bool FilterPlan::next()
{
	while (m_pPlan->next())
	{
		bool bMatch = true;
		for (size_t i = 0; i < m_predicate.size(); ++i)
		{
			PredicateInfo& info = m_predicate[i];
			int iSubIndex = info.m_iSubIndex;
			DBDataType type = m_pPlan->getResultType(iSubIndex);
			ResultInfo result;
			m_pPlan->getResult(iSubIndex, &result);

			if (result.m_bNull)
			{
				bMatch = false;
				break;
			}
			if (type == TYPE_STRING && info.m_iOpCode == LIKE)
			{
				if (info.m_pValue->m_iType != STR_NODE)
				{
					PARSE_ERROR("Wrong data type for %s, expect string",
							info.m_pValue->m_pszValue);
				}
				if (!doLike(result.m_value.m_pszResult, result.m_len,
						info.m_pValue->m_pszValue, info.m_pValue->m_iValue))
				{
					bMatch = false;
					break;
				}
			}
			else
			{
				int n = result.compare(info.m_pValue, type);
				if (!checkFilter(info.m_iOpCode, n))
				{
					bMatch = false;
					break;
				}
			}
		}
		if (!bMatch)
			continue;
		++m_iCurrent;
		return true;
	}
	return false;
}