#include "ExecutionException.h"
#include "oceanbase.h"

ExecutionException::ExecutionException(const char* pszMsg, bool appendOBError)
		: m_sErrMsg(pszMsg)
{
	if (appendOBError)
	{
		m_sErrMsg += ":";
		m_sErrMsg += ob_error();
	}
}

ExecutionException::~ExecutionException()
{
}
