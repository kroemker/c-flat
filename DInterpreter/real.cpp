#include "real.h"

namespace cflat
{

Real::Real(REALNUM v) : Token(ttypes::REALLIT)
{
	value = v;
}

const char* Real::tostring()
{
	char* s = new char[16]; 
	sprintf(s,"%f",value); 
	return s;
}

}