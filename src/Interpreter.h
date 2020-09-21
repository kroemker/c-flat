#pragma once

#include <vector>
#include "symbols.h"
#include "Lexer.h"
#include "Parser.h"
#include "Instruction.h"

namespace cflat
{

    class Interpreter
    {
    public:
        Interpreter();
        ~Interpreter();

        void registerFunction(char* name, ExternalFunctionPtr ptr);
        int parseFile(char* name);
        int execute(char* entryPoint);
		int callFunction(char* name, int nargs, ...);
		int initStack();

        StackEntry getReturnValue();
        char* getError();

        void printInstructions();
        void printStack();
        
    private:
		void pushValue(int val);
		void popValues(int num);

        Lexer* lexer;
        Parser* parser;
        Stack stack;
    };

}