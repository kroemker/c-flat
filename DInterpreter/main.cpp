
#include <iostream>
#include <cstdio>
#include "Lexer.h"
#include "Parser.h"
#include "exception.h"

using namespace inter;
using namespace std;

int main(int argc,char* argv[]) //* Eintrittsfunktion *//
{
	if (argc > 1) // auf programmargumente überprüfen
	{
		int iSourceSize = 0;
		FILE*   sSource;
		sSource = fopen(argv[1], "rb");
		if (sSource)
		{
			//Dateigröße bestimmen
			fseek(sSource, 0, SEEK_END);
			iSourceSize = ftell(sSource);
			fseek(sSource, 0, SEEK_SET);
			//Speicherreservierung für pRawContent
			unsigned char*			pRawContent = new unsigned char[iSourceSize + 1];
			fread(pRawContent, 1, iSourceSize, sSource);
			pRawContent[iSourceSize] = '\0';
			fclose(sSource);
			// Objekterzeugung der Klasse Lexer -> pLexer
			Lexer*					pLexer = new Lexer(pRawContent);
			pLexer->prelex();
			// Objekterzeugung der Klasse Parser -> pParser
			Parser*					pParser = new Parser(pLexer);
			inter::exception*		exc;
			while (!pLexer->endofstream())
			{
				exc = pParser->parsenext();
				if (exc != NULL)
					cout << "\n" << "Ausdruck Nr.  " << exc->getexprnum() << "\nFehler: " << exc->tostring();
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