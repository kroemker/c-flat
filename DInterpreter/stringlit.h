#pragma once

#include "token.h"
#include "symbols.h"

namespace cflat
{

class Stringlit : public Token
{
public:
	Stringlit(void);
	virtual ~Stringlit(void);
	void				append(char n);
	char*				get(){return buffer;}
	virtual const char*	tostring(){return (const char*)get();}
private:
	char*	buffer;
	int		position;
};

}