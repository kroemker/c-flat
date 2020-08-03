#include "token.h"

namespace inter
{

token::token(int t) : type(t)
{
}

const char* token::tostring()
{
	if(type>255) 
		return ttypes_s[type-256]; 
	else 
	{
		char* buf = new char[2]; // 2 Zeichen 'x','\0'
		buf[0] = static_cast<char>(type);
		buf[1] = '\0';
	}
}

} // namespace
