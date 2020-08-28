#pragma once

#include "symbols.h"

namespace cscript
{

class Exception
{
public:
	Exception(int type,int exprn){itype = type; exprnum = exprn;}
	virtual		~Exception(void){};
	int			gettype(){return itype;}
	int			getexprnum(){return exprnum;}
	const char*	tostring(){return texceptions_s[itype];}
private:
	int itype;
	int exprnum;
};

}