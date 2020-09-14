#include "token.h"

namespace cflat
{

	Token::Token(int t) : type(t)
	{
	}

	const char* Token::tostring()
	{
		if (type > 255)
			return ttypes_s[type - 256];
		else
		{
			char* buf = new char[2];
			buf[0] = static_cast<char>(type);
			buf[1] = '\0';
			return buf;
		}
	}

}
