/*
   Klasse: variable
   Namensraum: inter
   Beschreibung: Stellt eine Variable dar
*/
#pragma once

#include "symbols.h"

namespace inter
{

class variable
{
public:
	variable(int t,char* n);
	virtual ~variable(void);
	bool    getbval();
	int		getival();
	REALNUM	getrval();
	void	setval(bool b);
	void	setval(int i);
	void	setval(REALNUM r);
	int		gettype(){return type;}
	char*	getname(){return name;}
private:
	char*	 name; // Quellname
	void*	 val;  // Wert
	int		 type; // Datentyp 
};

} // namespace