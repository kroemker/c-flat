#include "Interpreter.h"

#include <cstdio>
#include <cassert>

namespace cflat
{
        Interpreter::Interpreter() : stack(NULL)
        {
            lexer = new Lexer();
            parser = new Parser(lexer);
        }

        Interpreter::~Interpreter()
        {
            SAFEDEL(parser);
			SAFEDEL(lexer);
        }

        void Interpreter::registerFunction(char* name, ExternalFunctionPtr ptr)
        {
            if (parser != NULL)
                parser->registerExternalFunction(name, ptr);
        }

        int Interpreter::parseFile(char* name)
        {
		    long fileSize = 0;
            FILE* f;
            f = fopen(name, "rb");
            if (!f)
                return 0;

            fseek(f, 0, SEEK_END);
            fileSize = ftell(f);
            fseek(f, 0, SEEK_SET);

			char* content = new char[fileSize + 1];
			fread(content, 1, fileSize, f);
			content[fileSize] = '\0';
			fclose(f);

			lexer->prelex(content, fileSize);
			parser->parse();

            delete[] content;
            return 1;
        }

        int Interpreter::callFunction(char* name)
        {
            if (parser == NULL)
                return 0;

			if (parser->setEntryPoint(name) == 0)
			{
				printf("Entry point '%s' not found...", name);
				return 0;
			}

   			parser->instructions[0].execute(&stack);
			while(1)
			{
				assert(PC < parser->instructions.size());
				//printf("PC: %04d, SP: %04d, OPC:\t", PC, SP);
				//parser->instructions[PC].print();
				parser->instructions[PC].execute(&stack);
				if (stack == NULL)
					break;
			};

            return 1;
        }

        char* Interpreter::getError()
        {
            if (parser != NULL && parser->isError())
                return parser->getErrorString();
            else
                return NULL;
        }

        void Interpreter::printInstructions()
        {
			for (int i = 0; i < parser->instructions.size(); i++)
			{
				printf("%d:\t", i);
				parser->instructions[i].print();
			}
        }

        void Interpreter::printStack()
        {
            for (size_t i = STACK_NUM_ENTRIES - 1; i >= SP / STACK_ENTRY_SIZE; i--)
            {
                printf("%*X: %08X\n", 6, i * STACK_ENTRY_SIZE, stack[i]);
            }
        }
}
