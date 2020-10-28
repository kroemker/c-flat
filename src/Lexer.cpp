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
		while (!isReady() && ((t = getNextToken()) != NULL))
		{
			t->setLine(line);
			tokens.push_back(t);
		}
		prelexed = true;
	}

	int Lexer::getDigit(char c, bool allowHexDigits)
	{
		const int vdigits[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,10,11,12,13,14,15 };
		const char digits[] = "0123456789ABCDEFabcdef";
		for (int i = 0; i < 10 + allowHexDigits * 12; i++)
		{
			if (c == digits[i])
				return vdigits[i];
		}
		return -1;
	}

	Token* Lexer::parseNumber()
	{
		int base = 10;
		int start;
		bool isFloat = false;

		union Number {
			int i;
			float f;
		} number = { 0 };

		// check base
		if (input[inputIndex] == '0')
		{
			inputIndex++;
			if (!isReady() && input[inputIndex] == 'b')
			{
				base = 2;
				inputIndex++;
			}
			else if (!isReady() && input[inputIndex] == 'x')
			{
				base = 16;
				inputIndex++;
			}
		}

		start = inputIndex;
		while (!isReady() && getDigit(input[inputIndex], base == 16) != -1)
			inputIndex++;

		// check for '.' and trailing 'f'
		int dot = inputIndex;
		if (!isReady() && input[inputIndex] == '.')
		{
			isFloat = true;
			inputIndex++;
			while (!isReady() && getDigit(input[inputIndex], base == 16) != -1)
				inputIndex++;
		}

		if (!isReady() && input[inputIndex] == 'f')
		{
			isFloat = true;
			inputIndex++;
		}

		// parse pre-dot digits
		Number curbase; 
		if (isFloat) curbase.f = 1; else curbase.i = 1;
		for (int i = dot - 1; i >= start; i--)
		{
			if (isFloat)
			{
				number.f += curbase.f * (float)(getDigit(input[i], base == 16));
				curbase.f *= base;
			}
			else
			{
				number.i += curbase.i * (float)(getDigit(input[i], base == 16));
				curbase.i *= base;
			}
		}
		
		// parse post-dot digits
		curbase.f = 1;
		for (int i = dot + 1; i < inputIndex; i++)
		{
			if (input[i] == 'f') 
				continue;

			number.f += (1.0f / curbase.f) * (float)(getDigit(input[i], base == 16));
			curbase.f *= base;
		}

		if (isFloat)
			return new Float(number.f);
		else
			return new Integer(number.i);
	}

	Token* Lexer::getNextToken()
	{
		if (isReady()) return NULL;

		char cCurrent = input[inputIndex];
		if (cCurrent == ' ' || cCurrent == '\r' || cCurrent == '\t' || cCurrent == '\n')
		{
			if (cCurrent == '\n') line++;
			inputIndex++;
			return getNextToken();
		}

		switch (cCurrent)
		{
		case '/':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '/')
			{
				while (!isReady() && input[inputIndex] != '\n')
					inputIndex++;
				line++;
			}
			else if (!isReady() && input[inputIndex] == '*')
			{
				while (!isReady())
				{
					inputIndex++;
					if (!isReady() && input[inputIndex] == '*')
					{
						inputIndex++;
						if (!isReady() && input[inputIndex] == '/')
							break;
					}
					if (!isReady() && input[inputIndex] == '\n')
						line++;
				}
			}
			else
			{
				inputIndex++;
				if (!isReady() && input[inputIndex] == '=')
				{
					inputIndex++;
					return new Token(TokenTypes::ASSIGN_DIV);
				}
				else
					return new Token('/');
			}
			inputIndex++;
			return getNextToken();
		case '\"':
		{
			inputIndex++;
			Stringlit* slit = new Stringlit();
			while (!isReady() && input[inputIndex] != '\"')
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
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::EQUALS);
			}
			else
				return new Token('=');
		case '!':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::UNEQUALS);
			}
			else
				return new Token('!');
		case '+':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '+')
			{
				inputIndex++;
				return new Token(TokenTypes::INCREMENT);
			}
			else if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_ADD);
			}
			else
				return new Token('+');
		case '-':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '-')
			{
				inputIndex++;
				return new Token(TokenTypes::DECREMENT);
			}
			else if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_SUB);
			}
			else
				return new Token('-');
		case '*':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_MUL);
			}
			else
				return new Token('*');
		case '%':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_MOD);
			}
			else
				return new Token('%');
		case '|':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_OR);
			}
			else
				return new Token('|');
		case '&':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_AND);
			}
			else
				return new Token('&');
		case '^':
			inputIndex++;
			if (!isReady() && input[inputIndex] == '=')
			{
				inputIndex++;
				return new Token(TokenTypes::ASSIGN_XOR);
			}
			else
				return new Token('^');
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
				if (!isReady() && input[inputIndex] == '=')
				{
					inputIndex++;
					return new Token(TokenTypes::ASSIGN_SHIFTL);
				}
				else
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
				if (!isReady() && input[inputIndex] == '=')
				{
					inputIndex++;
					return new Token(TokenTypes::ASSIGN_SHIFTR);
				}
				else
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
			while (!isReady())
				if (isalnum(input[inputIndex]) || input[inputIndex] == '_')
					id->append(input[inputIndex++]);
				else
					break;
			id->checkKeyword();
			return id;
		}
		else if (isdigit(cCurrent))
		{
			return parseNumber();
		}
		else
		{
			inputIndex++;
			return new Token(cCurrent);
		}
		return NULL;
	}
}