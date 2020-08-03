#include "identifier.h"

namespace inter
{

identifier::identifier(char c) : token(ttypes::ID)
{
	position = 1;
	buffer = NULL;
	buffer = new char[ID_SIZE_LIMIT];
	buffer[0] = c;
	buffer[1] = '\0';
}

identifier::~identifier(void)
{
	if(buffer)
		delete buffer;
	buffer = NULL;
}

void identifier::append(char n)
{
	buffer[position] = n; // keine überprüfung auf arraygrenzen 
	buffer[position+1] = '\0';
	position++;
}

void identifier::checkkeyword()
{
	int i;
	for(i = 0; i < KEYWORDCOUNT; i++)
		if(strcmp(static_cast<const char*> (this->get()), static_cast<const char*>( keywords_s[i])) == 0)
		{
			this->settype(KEYWORDTYPES + i);
			break;
		}
}

} // namespace