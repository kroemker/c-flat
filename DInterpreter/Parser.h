#pragma once

#include <map>
#include <list>
#include "symbols.h"
#include "Lexer.h"
#include "variable.h" 
#include "token.h"
#include "exception.h"

namespace cscript
{

	class Parser
	{
	public:
		Parser(Lexer* pLex);
		Exception*	parsenext();
		virtual ~Parser(void);
	private:
		Variable*					boolexpr();
		Variable*					joinexpr();
		Variable*					equalityexpr();
		Variable*					relexpr();
		Variable*					mathexpr(int numerictype, bool isboolexpr);
		Variable*					mathterm(int numerictype, bool isboolexpr);
		Variable*					mathunary(int numerictype, bool isboolexpr);
		Variable*					factor(int numerictype, bool isboolexpr);
		void						stmts();
		void						decl();
		void						decls();
		std::list<Variable*>		variables;
		Lexer*						pLexer;
	};

}