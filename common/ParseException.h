#pragma once

#include <exception>
#include <absl/strings/str_cat.h>

class ParseResult;
class ParseException: public std::exception {
public:
	ParseException(const std::string& msg);
	ParseException(ParseResult& pr);

	const char * what() const throw() override {
		return m_sErrorMsg.c_str();
	}

	int getLine() const {
		return m_iLine;
	}
	int getStartPos() const {
		return m_iStartCol;
	}
	void printLocation();
private:
	std::string m_sErrorMsg;
	int m_iStartCol = -1;
	int m_iEndCol = -1;
	int m_iLine = -1;
};

#define PARSE_ERROR(args...) {auto sError = absl::StrCat(args);LOG(ERROR)<<sError;throw ParseException(sError);}




