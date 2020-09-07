
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
		int fileSize = 0;
		FILE*   f;
		f = fopen(argv[1], "rb");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			fileSize = ftell(f);
			fseek(f, 0, SEEK_SET);

			char* content = new char[fileSize + 1];
			fread(content, 1, fileSize, f);
			content[fileSize] = '\0';
			fclose(f);

			Lexer* lexer = new Lexer(content, fileSize);
			lexer->prelex();
			Parser* parser = new Parser(lexer);
			cflat::Exception* exc;
			while (!lexer->isEndOfTokenList())
			{
				exc = parser->parsenext();
				if (exc != NULL)
					cout << "\n" << "Line: " << exc->gettoken()->getline() << ", Expression: " << exc->getexprnum() << " @ " << exc->gettoken()->tostring() << "\nError: " << exc->tostring() << "\n";
			}
			for (int i = 0; i < parser->instructions.size(); i++)
			{
				cout << i << ":\t";
				parser->instructions[i].print();
			}
			SAFEDEL(lexer);
			SAFEDEL(parser);
			delete[] content;
		}
		else
			cout << argv[1] << " could not be opened!";
	}
	else
		cout << "No file specified.";
	return 0;
}