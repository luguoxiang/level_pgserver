#pragma once


#include "common/Exception.h"



class BDException: public Exception {
public:
	BDException(const std::string& sMsg);

	virtual ~BDException() {
	}

	std::string what() const override {
		return m_sErrMsg;
	}

	BDException& operator=(const BDException& ex) = delete;
	BDException(const BDException& ex) = delete;

private:
	std::string m_sErrMsg;
};

#define DB_ERROR(args...) {auto sError = ConcateToString(args);LOG(ERROR)<<sError;throw new BDException(sError);}


