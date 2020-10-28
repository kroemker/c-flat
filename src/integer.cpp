#include "integer.h"

namespace cflat
{

	Integer::Integer(int v) : Token(TokenTypes::INT_LITERAL), value(v)
	{
	}

	const char* Integer::toString()
	{
		char* s = new char[16];
		sprintf(s, "%d", value);
		s[15] = '\0';
		return s;
	}

}