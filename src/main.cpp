
#include <cstdio>
#include <cassert>
#include "symbols.h"
#include "Interpreter.h"

using namespace cflat;
using namespace std;

void EXT_printTop(Stack stack)
{
	printf("Stack top: %d\n", STACK(12));
}

int main(int argc, char* argv[])
{
	Interpreter interpreter;
	char* error;
	if (argc > 1)
	{
		interpreter.registerFunction("printTop", EXT_printTop);
		if (interpreter.parseFile(argv[1]))
		{
			if (error = interpreter.getError())
			{
				printf("%s\n", error);
				return 1;
			}

			interpreter.printInstructions();

			interpreter.callFunction("main");
		}
		else
		{
			printf("%s could not be opened!", argv[1]);
			return 1;
		}
	}
	else
		printf("No file specified.");
	return 0;
}