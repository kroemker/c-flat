#include "StringLiteral.h"

namespace cflat
{

	StringLiteral::StringLiteral(void) : Token(TokenTypes::STRING)
	{
		position = 0;
		buffer = NULL;
		buffer = new char[ID_SIZE_LIMIT];
		buffer[0] = '\0';
	}

	StringLiteral::~StringLiteral(void)
	{
		SAFEDEL(buffer);
	}

	void StringLiteral::append(char n)
	{
		buffer[position] = n;
		buffer[position + 1] = '\0';
		position++;
	}
}