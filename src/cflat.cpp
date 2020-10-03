
#include <cstdio>
#include <cassert>
#include <cstdarg>

#include "cflat.h"

#include "symbols.h"
#include "function.h"
#include "Lexer.h"
#include "Parser.h"
#include "Optimizer.h"

using namespace cflat;

static Lexer* lexer;
static Parser* parser;
static Stack stack;

void pushValue(int value)
{
    SP -= STACK_ENTRY_SIZE;
    STACK(0) = value;
}

void popValues(int num)
{
    SP += num * STACK_ENTRY_SIZE;
}

void cfOpen()
{
    stack = NULL;
    lexer = new Lexer();
    parser = new Parser(lexer);
}

void cfClose()
{
    SAFEDELA(stack);
    SAFEDEL(parser);
    SAFEDEL(lexer);
}

void cfRegisterFunction(char* name, cfExternalFunctionPtr ptr)
{
    if (parser != NULL)
        parser->registerExternalFunction(name, (cflat::ExternalFunctionPtr)ptr);
}

int cfParseFile(char* name)
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

int cfOptimize()
{
	if (parser == NULL)
		return 0;

	Optimizer opt(parser);
	opt.optimize();

	return 1;
}

int cfExecute()
{
    if (parser == NULL)
        return 0;

    while(1)
    {
        assert(PC < parser->instructions.size());
        Instruction* instr = &parser->instructions[PC];
        //printf("PC: %04d, SP: %04d, OPC:\t", PC, SP);
        //instr->print();
        instr->execute(stack);
        if (instr->getOpcode() == Opcodes::Q)
            break;
    };

    return 1;
}

int cfInitStack(int size)
{
    if (parser == NULL)
        return 0;
    
    stack = new StackEntry[(size + sizeof(StackContext)) / sizeof(StackEntry)];
    StackContext* stackCtx = (StackContext*)stack;
    stackCtx->size = size;
    stackCtx->entries = size / sizeof(StackEntry);
    stackCtx->sp = size - parser->getGlobalStackSize();
    stackCtx->pc = 0;

    return 1;
}

int cfCallFunction(char* name, int nargs, ...)
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
        Instruction* instr = &parser->instructions[PC];
        //printf("PC: %04d, SP: %04d, OPC:\t", PC, SP);
        //instr.print();
        instr->execute(stack);
        if (instr->getOpcode() == Opcodes::YLD)
            break;
    };

    popValues(3 + nargs);

    return 1;
}

int cfGetReturnInteger()
{
    return STACK(-STACK_ENTRY_SIZE).i;
}

float cfGetReturnFloat()
{
    return STACK(-STACK_ENTRY_SIZE).f;
}

int cfGetArgumentCount()
{
    return STACK(STACK_ENTRY_SIZE).i;
}

int cfGetIntegerArgument(int num)
{
    return STACK(STACK_ENTRY_SIZE * (1 + cfGetArgumentCount() - num)).i;
}

float cfGetFloatArgument(int num)
{
    return STACK(STACK_ENTRY_SIZE * (1 + cfGetArgumentCount() - num)).f;
}

int cfGetReturnAddress()
{
    return STACK(0).i;
}

void cfReturnInteger(int v)
{
    STACK(STACK_ENTRY_SIZE * (1 + cfGetArgumentCount() + 1)).i = v;
}

void cfReturnFloat(float v)
{
    STACK(STACK_ENTRY_SIZE * (1 + cfGetArgumentCount() + 1)).f = v;
}

char* cfGetError()
{
    if (parser != NULL && parser->isError())
        return parser->getErrorString();
    else
        return NULL;
}

void cfPrintInstructions()
{
    for (int i = 0; i < parser->instructions.size(); i++)
    {
        printf("%d:\t", i);
        parser->instructions[i].print();
    }
}

void cfPrintStack()
{
    for (size_t i = ((StackContext*)stack)->entries - 1; i >= SP / STACK_ENTRY_SIZE; i--)
    {
        printf("%*X: %08X\n", 6, i * STACK_ENTRY_SIZE, stack[i]);
    }
}