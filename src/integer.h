#pragma once

#include "token.h"
#include <cstdlib>
#include <cstdio>

namespace cflat
{

class Integer : public Token
{
public:
	Integer(int v);
	virtual ~Integer(void){};
	void				setValue(int v){value = v;}
	int					getValue(){return value;}
	virtual const char* toString();
private:
	int value;
};

}