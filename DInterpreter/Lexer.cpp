#include "Lexer.h"

namespace cscript
{

	Lexer::Lexer(char* str, int len) : input(str), inputLength(len), inputIndex(0), numExpr(0), tokenIndex(0), prelexed(false)
	{
	}

	Lexer::~Lexer(void)
	{
	}

	void Lexer::prelex()
	{
		Token* t;
		while (!isready() && ((t = getnexttoken()) != NULL))
			tokens.push_back(t);
		prelexed = true;
	}

	Token* Lexer::getnexttoken()
	{
		if (isready()) return NULL;

		char cCurrent = input[inputIndex];
		if (cCurrent == ' ' || cCurrent == '\r' || cCurrent == '\t' || cCurrent == '\n')
		{
			inputIndex++;
			return getnexttoken();
		}

		switch (cCurrent)
		{
		case '/':
			inputIndex++;
			if (!isready() && input[inputIndex] == '/')
			{
				while (!isready() && input[inputIndex] != '\n')
					inputIndex++;
			}
			else if (!isready() && input[inputIndex] == '*')
			{
				while (!isready())
				{
					if (!isready() && input[inputIndex] != '*')
					{
						inputIndex++;
						if (!isready() && input[inputIndex] == '/')
							break;
					}
				}
			}
			inputIndex++;
			return getnexttoken();
		case '\"':
		{
			inputIndex++;
			Stringlit* slit = new Stringlit();
			while (input[inputIndex] != '\"')
			{
				slit->append(input[inputIndex]);
				inputIndex++;
			}
			inputIndex++;
			return slit;
		}
		case '=':
			inputIndex++;
			if (!isready() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(ttypes::EQUALS);
			}
			else
				return new Token('=');
		case '!':
			inputIndex++;
			if (input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(ttypes::UNEQUALS);
			}
			else
				return new Token('!');
		case '<':
			inputIndex++;
			if (input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(ttypes::LESS_EQUALS);
			}
			else
				return new Token('<');
		case '>':
			inputIndex++;
			if (input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(ttypes::GREATER_EQUALS);
			}
			else
				return new Token('>');
			break;
		}
		if (isalpha(cCurrent) || cCurrent == '_')
		{
			Identifier* id = new Identifier(tolower(cCurrent));
			inputIndex++;
			while (!isready())
				if (isalnum(input[inputIndex]) || input[inputIndex] == '_')
					id->append(tolower(input[inputIndex++]));
				else
					break;
			id->checkkeyword();
			return id;
		}
		else if (isdigit(cCurrent))
		{
			int start = inputIndex;
			bool isReal = false;
			inputIndex++;
			while (!isready())
				if (isdigit(input[inputIndex]))
					inputIndex++;
				else
					break;
			if (!isready())
				if (input[inputIndex] == '.')
				{
					isReal = true;
					inputIndex++;
					while (!isready())
						if (isdigit(input[inputIndex]))
							inputIndex++;
						else
							break;
				}
			char* buffer = new char[inputIndex - start + 1];
			for (unsigned int i = 0; i < inputIndex - start; i++)
				buffer[i] = input[start + i];
			buffer[inputIndex - start] = '\0';
			if (isReal)
				return new Real(atof(buffer));
			else
				return new Integer(atoi(buffer));
		}
		else
		{
			inputIndex++;
			return new Token(cCurrent);
		}
		return NULL;
	}

}