#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "token.h"
#include "real.h"
#include "identifier.h"
#include "integer.h"
#include "stringlit.h"

namespace cscript
{

	class Lexer
	{
	public:
		Lexer(char* str, int len);
		virtual ~Lexer(void);
		void	next() { if (isEndOfTokenList()) return; if (tokens[tokenIndex]->gettype() == ';')numExpr++; tokenIndex++; }
		void	prelex();
		void	jump(int pos) { tokenIndex = pos; }
		int		getExpressionCount() { return numExpr; }
		int		getTokenIndex() { return tokenIndex; }
		Token*  look() { return tokens[tokenIndex]; }
		bool	isEndOfTokenList() { return(tokens.size() == tokenIndex); }
		bool	isPrelexed() { return prelexed; }
	private:
		bool				isready() { return inputIndex == inputLength; }
		Token*				getnexttoken();
		std::vector<Token*>	tokens;
		char*				input;
		unsigned int		tokenIndex;
		unsigned int		inputIndex;
		unsigned int		numExpr;
		bool				prelexed;
		int					inputLength;
	};
}