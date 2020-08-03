/*
   Klasse: integer
   Basisklasse: token
   Namensraum: inter
   Beschreibung: Beschreibt Integerliterale(INT)
*/
#pragma once

#include "token.h"
#include <cstdlib>
#include <cstdio>

namespace inter
{

class integer : public token
{
public:
	integer(int v);
	virtual ~integer(void){};
	void				setvalue(int v){value = v;}
	int					getvalue(){return value;}
	virtual const char* tostring();
private:
	int value;
};

} // namespace