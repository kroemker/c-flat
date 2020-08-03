/*
   Klasse: real
   Basisklasse: token
   Namensraum: inter
   Beschreibung: Beschreibt Realliterale(REALLIT)
*/
#pragma once

#include "token.h"
#include "symbols.h"
#include <string>

namespace inter
{

class real : public token
{
public:
	real(REALNUM v);
	virtual ~real(void){};
	void				setvalue(REALNUM v){value = v;}
	REALNUM				getvalue(){return value;}
	virtual const char*	tostring();
private:
	REALNUM value;
};

} // namespace