#include "execution/LimitPlan.h"
#include "common/ParseException.h"

bool LimitPlan::next() {
	while (m_iCurrent < m_iOffset) {
		if (!m_pPlan->next())
			return false;
		++m_iCurrent;
	}
	if (!m_pPlan->next())
		return false;
	return (m_iCurrent++) < m_iLimit + m_iOffset;
}
