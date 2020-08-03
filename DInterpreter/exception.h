/*
   Klasse: exception
   Namensraum: inter
   Beschreibung: Verarbeitet Code Ausnahmen
*/
#pragma once

#include "symbols.h"

namespace inter
{

class exception
{
public:
	exception(int type,int exprn){itype = type; exprnum = exprn;}
	virtual		~exception(void){};
	int			gettype(){return itype;}
	int			getexprnum(){return exprnum;}
	const char*	tostring(){return texceptions_s[itype];}
private:
	int itype;
	int exprnum;
};

} // namespace