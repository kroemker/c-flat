#pragma once

#include <map>
#include <list>
#include "symbols.h"
#include "Lexer.h"
#include "variable.h" 
#include "token.h"
#include "exception.h"

namespace inter
{

class Parser
{
public:
	Parser(Lexer* pLex);
	exception*	parsenext();
	virtual ~Parser(void);
private:
	variable*					boolexpr();
	variable*					joinexpr();
	variable*					equalityexpr();
	variable*					relexpr();
	variable*					mathexpr(int numerictype,bool isboolexpr);
	variable*					mathterm(int numerictype,bool isboolexpr);
	variable*					mathunary(int numerictype,bool isboolexpr);
	variable*					factor(int numerictype,bool isboolexpr);
	void						stmts();
	void						decl();
	void						decls();
	std::list<variable*>		variables;
	Lexer*						pLexer;
};

} // namespace