#include "real.h"

namespace inter
{

real::real(REALNUM v) : token(ttypes::REALLIT)
{
	value = v;
}

const char* real::tostring()
{
	char* s = new char[16]; 
	sprintf(s,"%f",value); 
	return s;
}

} // namespace