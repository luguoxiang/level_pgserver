#pragma once

#include "ParseNode.h"
#include "Exception.h"
#include <string>
#include <stdio.h>

class ParseException: public Exception {
public:
	ParseException(const std::string& msg);
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
	int m_iStartCol = 0;
	int m_iEndCol = 0;
	int m_iLine = -1;
};

