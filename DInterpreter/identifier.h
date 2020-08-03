/*
   Klasse: identifier
   Basisklasse: token
   Namensraum: inter
   Beschreibung: Beschreibt Textsymbole (ID,Keywords)
*/
#pragma once

#include "token.h"
#include "symbols.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace inter
{


class identifier : public token
{
public:
	identifier(char c);
	virtual ~identifier(void);
	void				append(char n);
	char*				get(){return buffer;}
	void				checkkeyword();
	virtual const char*	tostring(){return static_cast<const char*>(get());}
private:
	char*	buffer;
	int		position;
};

} // namespace