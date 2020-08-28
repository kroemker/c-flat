#pragma once

#include "token.h"
#include <cstdlib>
#include <cstdio>

namespace cscript
{

class Integer : public Token
{
public:
	Integer(int v);
	virtual ~Integer(void){};
	void				setvalue(int v){value = v;}
	int					getvalue(){return value;}
	virtual const char* tostring();
private:
	int value;
};

}