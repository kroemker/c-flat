/*
   Klasse: stringlit
   Basisklasse: token
   Namensraum: inter
   Beschreibung: Beschreibt Stringliteral(STRING)
*/
#pragma once

#include "token.h"
#include "symbols.h"

namespace inter
{

class stringlit : public token
{
public:
	stringlit(void);
	virtual ~stringlit(void);
	void				append(char n);
	char*				get(){return buffer;}
	virtual const char*	tostring(){return (const char*)get();}
private:
	char*	buffer;
	int		position;
};

} // namespace