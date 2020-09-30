#include "Lexer.h"

namespace cflat
{

	Lexer::Lexer() : input(NULL), inputLength(0), inputIndex(0), numExpr(0), tokenIndex(0), prelexed(false), line(1)
	{
	}

	Lexer::~Lexer(void)
	{
	}

	void Lexer::prelex(char* str, int len)
	{
		input = str;
		inputLength = len;
		Token* t;
		while (!isready() && ((t = getnexttoken()) != NULL))
		{
			t->setline(line);
			tokens.push_back(t);
		}
		prelexed = true;
	}

	Token* Lexer::getnexttoken()
	{
		if (isready()) return NULL;

		char cCurrent = input[inputIndex];
		if (cCurrent == ' ' || cCurrent == '\r' || cCurrent == '\t' || cCurrent == '\n')
		{
			if (cCurrent == '\n') line++;
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
				line++;
			}
			else if (!isready() && input[inputIndex] == '*')
			{
				while (!isready())
				{
					inputIndex++;
					if (!isready() && input[inputIndex] == '*')
					{
						inputIndex++;
						if (!isready() && input[inputIndex] == '/')
							break;
					}
					if (!isready() && input[inputIndex] == '\n')
						line++;
				}
			}
			else
			{
				inputIndex++;
				return new Token('/');
			}
			inputIndex++;
			return getnexttoken();
		case '\"':
		{
			inputIndex++;
			Stringlit* slit = new Stringlit();
			while (!isready() && input[inputIndex] != '\"')
			{
				if (input[inputIndex] == '\n') line++;
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
				return new Token(TokenTypes::EQUALS);
			}
			else
				return new Token('=');
		case '!':
			inputIndex++;
			if (!isready() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::UNEQUALS);
			}
			else
				return new Token('!');
		case '+':
			inputIndex++;
			if (!isready() && input[inputIndex] == '+')
			{
				inputIndex++;
				return new Token(TokenTypes::INCREMENT);
			}
			else
				return new Token('+');
		case '-':
			inputIndex++;
			if (!isready() && input[inputIndex] == '-')
			{
				inputIndex++;
				return new Token(TokenTypes::DECREMENT);
			}
			else
				return new Token('-');
		case '<':
			inputIndex++;
			if (input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::LESS_EQUALS);
			}
			else if (input[inputIndex] == '<')
			{
				inputIndex++;
				return new Token(TokenTypes::SHIFT_LEFT);
			}
			else
				return new Token('<');
		case '>':
			inputIndex++;
			if (input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::GREATER_EQUALS);
			}
			else if (input[inputIndex] == '>')
			{
				inputIndex++;
				return new Token(TokenTypes::SHIFT_RIGHT);
			}
			else
				return new Token('>');
			break;
		}
		if (isalpha(cCurrent) || cCurrent == '_')
		{
			Identifier* id = new Identifier(cCurrent);
			inputIndex++;
			while (!isready())
				if (isalnum(input[inputIndex]) || input[inputIndex] == '_')
					id->append(input[inputIndex++]);
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
				return new Float(atof(buffer));
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