#pragma once
#include <string>
#include <sstream>
#include <glog/logging.h>

class Exception {
public:
	Exception();
	virtual ~Exception();
	virtual std::string what() const = 0;

	virtual int getLine() const {
		return -1;
	}

	virtual int getStartPos() const {
		return 0;
	}
	Exception(const Exception&) = delete;
	Exception& operator =(const Exception&) = delete;
};

template<typename ...Args>
inline std::string ConcateToString(Args&&...args) {
        std::ostringstream os;
        (os << ... << std::forward<Args>(args));
        return os.str();
}
