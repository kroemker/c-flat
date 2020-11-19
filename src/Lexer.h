#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "Token.h"
#include "Float.h"
#include "Identifier.h"
#include "Integer.h"
#include "StringLiteral.h"

namespace cflat
{

	class Lexer
	{
	public:
		Lexer();
		virtual ~Lexer(void);
		void	next() { if (isEndOfTokenList()) return; if (tokens[tokenIndex]->getType() == ';')numExpr++; tokenIndex++; }
		void	prelex(char* str, int len);
		void	jump(int pos) { tokenIndex = pos; }
		int		getExpressionCount() { return numExpr; }
		int		getTokenIndex() { return tokenIndex; }
		Token*  look() { return tokens[tokenIndex]; }
		Token*  lookahead(int t) { return tokens[tokenIndex + t]; }
		bool	isEndOfTokenList() { return(tokens.size() == tokenIndex); }
		bool	isPrelexed() { return prelexed; }
	private:
		bool				isReady() { return inputIndex >= inputLength; }
		Token*				getNextToken();
		Token*				parseNumber();
		int					getDigit(char c, bool allowHexDigits);
		std::vector<Token*>	tokens;
		char*				input;
		unsigned int		tokenIndex;
		unsigned int		inputIndex;
		unsigned int		numExpr;
		bool				prelexed;
		int					inputLength;
		int					line;
	};
}