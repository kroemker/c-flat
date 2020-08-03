#include "stringlit.h"

namespace inter
{

stringlit::stringlit(void) : token(ttypes::STRING)
{
	position = 0;
	buffer = NULL;
	buffer = new char[ID_SIZE_LIMIT];
	buffer[0] = '\0';
}

stringlit::~stringlit(void)
{
	SAFEDEL(buffer);
}

void stringlit::append(char n)
{
	buffer[position] = n; // keine überprüfung auf arraygrenzen 
	buffer[position+1] = '\0';
	position++;
}

} // namespace