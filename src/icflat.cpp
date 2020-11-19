#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "cflat.h"

void cfPrint(cfStack stack)
{
	for (size_t i = 0; i < cfGetArgumentCount(); i++)
	{
		printf("%d  ", cfGetIntegerArgument(i));
	}
	printf("\n");
}

int main(int argc, char* argv[])
{
	char* error;
	bool printInstrs = false;
	bool printResult = false;
	bool optimize = false;
	char functionToCall[128] = { 0 };
	int stackSize = 0x100000; // 1 MB

	for (size_t i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-p") == 0)
			printInstrs = true;
		else if (strcmp(argv[i], "-r") == 0)
			printResult = true;
		else if (strcmp(argv[i], "-o") == 0)
			optimize = true;
		else if (strcmp(argv[i], "-f") == 0)
			strcpy(functionToCall, argv[++i]);
		else if (strcmp(argv[i], "-s") == 0)
			stackSize = 1024 * atoi(argv[++i]);
	}

	if (argc > 1)
	{
		cfOpen();
		cfRegisterFunction("print", cfPrint);
		if (cfParseFile(argv[argc-1]))
		{
			if (error = cfGetError())
			{
				printf("%s\n", error);
				return 1;
			}

			if (optimize)
				cfOptimize();

			if (printInstrs)
				cfPrintInstructions();

			cfInitStack(stackSize);
			if (functionToCall[0])
			{
				cfCallFunction(functionToCall, 0);

				if (printResult)
					printf("Result: %d\n", cfGetReturnInteger());
			}
			else
				cfExecute();

		}
		else
		{
			printf("%s could not be opened!", argv[1]);
			return 1;
		}
		cfClose();
	}
	else
		printf("No file specified.");
	return 0;
}