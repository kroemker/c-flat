#pragma once

#include "symbols.h"
#include <cstdlib>

namespace cscript
{

class Token
{
public:
	Token(int t);
	virtual ~Token(void){};
	void				settype(int t)	{type = t;}
	int					gettype()		{return type;}
	virtual const char*	tostring();
private:
	int type;
};

}