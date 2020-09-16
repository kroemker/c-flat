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
        int callFunction(char* name);
        StackEntry getReturnValue();
        char* getError();
        void printInstructions();
        void printStack();
        
    private:
        Lexer* lexer;
        Parser* parser;
        Stack stack;
    };

}