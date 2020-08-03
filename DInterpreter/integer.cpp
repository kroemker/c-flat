#include "integer.h"

namespace inter
{

integer::integer(int v) : token(ttypes::INT),value(v)
{
}

const char* integer::tostring()
{
	char* s = new char[16]; 
	sprintf(s,"%d",value); 
		s[15] = '\0';
	return s;
}

} // namespace