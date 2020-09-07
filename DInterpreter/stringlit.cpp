#include "stringlit.h"

namespace cflat
{

	Stringlit::Stringlit(void) : Token(TokenTypes::STRING)
	{
		position = 0;
		buffer = NULL;
		buffer = new char[ID_SIZE_LIMIT];
		buffer[0] = '\0';
	}

	Stringlit::~Stringlit(void)
	{
		SAFEDEL(buffer);
	}

	void Stringlit::append(char n)
	{
		buffer[position] = n;
		buffer[position + 1] = '\0';
		position++;
	}
}