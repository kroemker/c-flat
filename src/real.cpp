#include "real.h"

namespace cflat
{

	Float::Float(float v) : Token(TokenTypes::FLOAT_LITERAL)
	{
		value = v;
	}

	const char* Float::tostring()
	{
		char* s = new char[16];
		sprintf(s, "%f", value);
		return s;
	}

}