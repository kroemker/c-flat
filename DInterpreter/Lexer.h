#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "token.h"
#include "real.h"
#include "identifier.h"
#include "integer.h"
#include "stringlit.h"

namespace inter
{

class Lexer
{
public:
	Lexer(unsigned char* stringstream);
	virtual ~Lexer(void);
	void	next() {if(tokenstream[streampos]->gettype() == ';')iexpr++; streampos++;}
	void	prelex();
	void	jumpto(int pos){streampos = pos;}
	int		getexprnum(){return iexpr;}
	int		getstreampos(){return streampos;}
	token*  look(){return tokenstream[streampos];}
	bool	endofstream(){return(tokenstream.size() == streampos);}
	bool	isprelexed(){return prelexed;}
private:
	bool				isready(){return (sInputStream[iPosition] == '\0');}
	token*				getnexttoken();
	std::vector<token*>	tokenstream;
	unsigned char*		sInputStream;
	unsigned int		streampos;
	unsigned int		iPosition;
	unsigned int		iexpr;
	bool				prelexed;
};
} // namespace