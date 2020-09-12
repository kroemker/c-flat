
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
			parser->parse();
			if (parser->isError())
				printf("%s\n", parser->getErrorString());
			for (int i = 0; i < parser->instructions.size(); i++)
			{
				printf("%d:\t", i);
				parser->instructions[i].print();
			}
			// execute
			printf("\nExecution:\n");
			Stack stack = NULL;
			parser->instructions[0].execute(&stack);
			do
			{
				printf("PC: %02d , SP: %03d \t", PC, SP);
				parser->instructions[PC].print();
				parser->instructions[PC].execute(&stack);
			} while (stack != NULL);

			SAFEDEL(lexer);
			SAFEDEL(parser);
			delete[] content;
		}
		else
			printf("%s could not be opened!", argv[1]);
	}
	else
		printf("No file specified.");
	return 0;
}