/*
   Klasse: token
   Namensraum: inter
   Beschreibung: Beschreibt Token
*/
#pragma once

#include "symbols.h"
#include <cstdlib>

namespace inter
{

class token
{
public:
	token(int t);
	virtual ~token(void){};
	void				settype(int t)	{type = t;}
	int					gettype()		{return type;}
	virtual const char*	tostring();
private:
	int type;
};

} // namespace