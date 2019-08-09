#pragma once

#include "ParseNode.h"
#include "Exception.h"
#include <string>
#include <stdio.h>

class ParseException: public Exception
{
public:
	ParseException(const char* pszMsg, ...);
	ParseException(ParseResult* pResult);

	virtual ~ParseException();

	const char* what() const
	{
		return m_sErrorMsg.c_str();
	}

	int getLine() const
	{
		return m_iLine;
	}
	int getStartPos() const
	{
		return m_iStartCol;
	}
	void printLocation();
private:
	ParseException(const ParseException& ex);
	ParseException& operator =(const ParseException& ex);

	std::string m_sErrorMsg;
	int m_iStartCol;
	int m_iEndCol;
	int m_iLine;
};

