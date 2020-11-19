#include "Float.h"

namespace cflat
{

	Float::Float(float v) : Token(TokenTypes::FLOAT_LITERAL)
	{
		value = v;
	}

	const char* Float::toString()
	{
		char* s = new char[16];
		sprintf(s, "%f", value);
		return s;
	}

}