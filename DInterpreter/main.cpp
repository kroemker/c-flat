
#include <iostream>
#include <cstdio>
#include "Lexer.h"
#include "Parser.h"
#include "exception.h"

using namespace cflat;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int iSourceSize = 0;
		FILE*   sSource;
		sSource = fopen(argv[1], "rb");
		if (sSource)
		{
			fseek(sSource, 0, SEEK_END);
			iSourceSize = ftell(sSource);
			fseek(sSource, 0, SEEK_SET);

			char* pRawContent = new char[iSourceSize + 1];
			fread(pRawContent, 1, iSourceSize, sSource);
			pRawContent[iSourceSize] = '\0';
			fclose(sSource);

			Lexer*	pLexer = new Lexer(pRawContent, iSourceSize);
			pLexer->prelex();

			Parser*	pParser = new Parser(pLexer);
			cflat::Exception*	exc;
			while (!pLexer->isEndOfTokenList())
			{
				exc = pParser->parsenext();
				if (exc != NULL)
					cout << "\n" << "Line: " << exc->getline() << ", Expression: " << exc->getexprnum() << "\nError: " << exc->tostring() << "\n";
			}
			SAFEDEL(pLexer);
			SAFEDEL(pParser);
			delete[] pRawContent;
		}
		else
			cout << argv[1] << " could not be opened!";
	}
	else
		cout << "No file specified.";
	return 0;
}