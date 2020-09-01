#pragma once

#include "symbols.h"

namespace cflat
{

class Variable
{
public:
	Variable(int t,char* n);
	virtual ~Variable(void);
	bool    getbval();
	int		getival();
	REALNUM	getrval();
	void	setval(bool b);
	void	setval(int i);
	void	setval(REALNUM r);
	int		gettype() { return type; }
	char*	getname() { return name; }
private:
	char*	 name;
	void*	 val;
	int		 type; 
};

}