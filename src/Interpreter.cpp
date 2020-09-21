#include "Interpreter.h"

#include <cstdio>
#include <cassert>
#include <cstdarg>
#include "function.h"

namespace cflat
{
        Interpreter::Interpreter() : stack(NULL)
        {
            lexer = new Lexer();
            parser = new Parser(lexer);
        }

        Interpreter::~Interpreter()
        {
			SAFEDELA(stack);
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

        int Interpreter::execute(char* entryPoint)
        {
            if (parser == NULL)
                return 0;

			if (parser->setEntryPoint(entryPoint) == 0)
			{
				printf("Entry point '%s' not found...", entryPoint);
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

		int Interpreter::initStack()
		{
			if (parser == NULL)
				return 0;

			stack = new StackEntry[STACK_NUM_ENTRIES];
			SP = STACK_SIZE - parser->getGlobalStackSize();
			PC = 0;

			return 1;
		}

		int Interpreter::callFunction(char* name, int nargs, ...)
		{
			va_list arguments;

            if (parser == NULL)
                return 0;

			Function* f = parser->getFunction(name);
			if (!f)
			{
				printf("Function '%s' not found...", name);
				return 0;
			}
			else if (f->isExternal())
			{
				printf("Function '%s' must not be external...", name);
				return 0;
			}

			PC = f->getAddress();

			// push return value space
			pushValue(0);

			// push arguments
			va_start(arguments, nargs);
			for (int i = 0; i < nargs; i++)
			{
				pushValue(va_arg(arguments, int));
			}
			va_end(arguments);

			// push nargs
			pushValue(nargs);

			// push return address
			pushValue(parser->instructions.size() - 1);

			while (1)
			{
				assert(PC < parser->instructions.size());
				Instruction instr = parser->instructions[PC];
				//printf("PC: %04d, SP: %04d, OPC:\t", PC, SP);
				//instr.print();
				instr.execute(&stack);
				if (instr.getOpcode() == Opcodes::YLD)
					break;
			};

			popValues(3 + nargs);

			return 1;
		}

		StackEntry Interpreter::getReturnValue()
		{
			return STACK(-4);
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

		void Interpreter::pushValue(int value)
		{
			SP -= STACK_ENTRY_SIZE;
			STACK(0) = value;
		}

		void Interpreter::popValues(int num)
		{
			SP += num * STACK_ENTRY_SIZE;
		}
}
