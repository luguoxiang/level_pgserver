#ifndef EXCEPTION_H
#define EXCEPTION_H

class Exception
{
public:
	Exception();
	virtual ~Exception();
	virtual const char* what() const = 0;

	virtual int getLine() const
	{
		return -1;
	}

	virtual int getStartPos() const
	{
		return 0;
	}
	;
};

#endif //EXCEPTION_H