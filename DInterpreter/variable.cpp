#include "variable.h"

namespace cscript
{

Variable::Variable(int t,char* n) : name(n),type(t)
{
	if(t == tresult::RES_BOOLEAN)
	{
		val = new bool;
	}
	else if(t == tresult::RES_INTEGER)
	{
		val = new int;
	}
	else if(t == tresult::RES_REAL)
	{
		val = new REALNUM;
	}
}

Variable::~Variable(void)
{
}
bool Variable::getbval()
{
	bool* b = reinterpret_cast<bool*>(val);
	return *b;
}

int Variable::getival()
{
	int* i = reinterpret_cast<int*>(val);
	return *i;
}

REALNUM Variable::getrval()
{
	REALNUM* f= reinterpret_cast<REALNUM*>(val);
	return *f;
}

void Variable::setval(bool b)
{
	bool* k = reinterpret_cast<bool*>(val);
	*k = b;
}

void Variable::setval(REALNUM r)
{
	REALNUM* k = reinterpret_cast<REALNUM*>(val);
	*k = r;
}

void Variable::setval(int i)
{
	int* k = reinterpret_cast<int*>(val);
	*k = i;
}

}