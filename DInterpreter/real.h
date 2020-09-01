#pragma once

#include "token.h"
#include "symbols.h"
#include <string>

namespace cflat
{

class Real : public Token
{
public:
	Real(REALNUM v);
	virtual ~Real(void){};
	void				setvalue(REALNUM v){value = v;}
	REALNUM				getvalue(){return value;}
	virtual const char*	tostring();
private:
	REALNUM value;
};

}