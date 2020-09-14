
#include <cstdio>
#include <cassert>
#include "symbols.h"
#include "Lexer.h"
#include "Parser.h"
#include "exception.h"

using namespace cflat;
using namespace std;

void EXT_printTop(Stack stack)
{
	printf("Stack top: %d\n", STACK(0));
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int fileSize = 0;
		FILE* f;
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
			parser->registerExternalFunction("printTop", EXT_printTop);
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
			while(1)
			{
				assert(PC < parser->instructions.size());
				printf("PC: %02d , SP: %04d, STACK(0): %08X \t", PC, SP, STACK(0));
				parser->instructions[PC].print();

				parser->instructions[PC].execute(&stack);
				if (stack == NULL)
					break;
				// print stack
				for (size_t i = STACK_NUM_ENTRIES - 1; i >= SP / STACK_ENTRY_SIZE; i--)
					printf("%08X ", stack[i]);
				printf("\n");
			};

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