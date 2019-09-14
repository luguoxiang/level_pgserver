#include "execution/LimitPlan.h"
#include "common/ParseException.h"

bool LimitPlan::next(const std::atomic_bool& bTerminated) {
	while (m_iCurrent < m_iOffset) {
		if (!m_pPlan->next(bTerminated))
			return false;
		++m_iCurrent;
	}
	if (!m_pPlan->next(bTerminated))
		return false;
	return (m_iCurrent++) < m_iLimit + m_iOffset;
}
