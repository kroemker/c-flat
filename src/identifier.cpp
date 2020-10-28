#include "identifier.h"

namespace cflat
{

	Identifier::Identifier(char c) : Token(TokenTypes::ID)
	{
		position = 1;
		buffer = NULL;
		buffer = new char[ID_SIZE_LIMIT];
		buffer[0] = c;
		buffer[1] = '\0';
	}

	Identifier::~Identifier(void)
	{
		if (buffer)
			delete buffer;
		buffer = NULL;
	}

	void Identifier::append(char n)
	{
		buffer[position] = n;
		buffer[position + 1] = '\0';
		position++;
	}

	void Identifier::checkKeyword()
	{
		int i;
		for (i = 0; i < KEYWORDCOUNT; i++)
			if (strcmp(static_cast<const char*> (this->get()), static_cast<const char*>(keywords_s[i])) == 0)
			{
				this->setType(KEYWORDTYPES + i);
				break;
			}
	}

}