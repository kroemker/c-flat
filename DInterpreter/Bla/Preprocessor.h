#pragma once

#include <list>
#include <algorithm>
#include "Lexer.h"
#include "token.h"
#include "variable.h"

class Preprocessor
{
public:
	Preprocessor(Lexer* p);
	virtual ~Preprocessor(void);
	void preprocess();
private:
	char*					registervar(char* k);
	void					searchandreplace(token* t);
	void					decls();
	void					decl();
	Lexer*					pLexer;
	std::list<variable*>	variables;
};
