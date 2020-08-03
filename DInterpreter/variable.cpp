#include "variable.h"

namespace inter
{

variable::variable(int t,char* n) : name(n),type(t)
{
	if(t == tresult::RES_BOOLEAN)
	{
		val = new bool; // 1 byte: 8 bit
	}
	else if(t == tresult::RES_INTEGER)
	{
		val = new int; // 4 byte: 32 bit
	}
	else if(t == tresult::RES_REAL)
	{
		val = new REALNUM; // 4/8 byte: 32/64 bit
	}
}

variable::~variable(void)
{
}

// getval- Funktionen prüfen nicht auf Richtigkeit der Typen!
bool variable::getbval()
{
	// Bitfolge in val neu interpretieren 
	// und einem Pointer zuweisen
	bool* b = reinterpret_cast<bool*>(val);
	// Inhalt ausgeben
	return *b;
}

int variable::getival()
{
	int* i = reinterpret_cast<int*>(val);
	return *i;
}

REALNUM variable::getrval()
{
	REALNUM* f= reinterpret_cast<REALNUM*>(val);
	return *f;
}

void variable::setval(bool b)
{
	bool* k = reinterpret_cast<bool*>(val);
	*k = b;
}

void variable::setval(REALNUM r)
{
	REALNUM* k = reinterpret_cast<REALNUM*>(val);
	*k = r;
}

void variable::setval(int i)
{
	int* k = reinterpret_cast<int*>(val);
	*k = i;
}

} // namespace