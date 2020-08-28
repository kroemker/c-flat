#include "integer.h"

namespace cscript
{

Integer::Integer(int v) : Token(ttypes::INT),value(v)
{
}

const char* Integer::tostring()
{
	char* s = new char[16]; 
	sprintf(s,"%d",value); 
		s[15] = '\0';
	return s;
}

}