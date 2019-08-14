#pragma once

#include "ParseNode.h"
#include "Exception.h"
#include <string>
#include <stdio.h>

class ParseException: public Exception {
public:
	ParseException(const char* pszMsg, ...);
	ParseException(ParseResult* pResult);

	virtual ~ParseException();

	std::string what() const override{
		return m_sErrorMsg;
	}

	int getLine() const override{
		return m_iLine;
	}
	int getStartPos() const override{
		return m_iStartCol;
	}
	void printLocation();

	ParseException(const ParseException& ex) = delete;
	ParseException& operator =(const ParseException& ex) = delete;

private:
	std::string m_sErrorMsg;
	int m_iStartCol;
	int m_iEndCol;
	int m_iLine;
};

