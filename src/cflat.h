#pragma once

union cfStackEntry;
typedef cfStackEntry* cfStack;
typedef void (*cfExternalFunctionPtr)(cfStack); 

void cfOpen();
void cfClose();

void cfRegisterFunction(char* name, cfExternalFunctionPtr ptr);
int cfInitStack(int size);
int cfParseFile(char* name);
int cfExecute(char* entryPoint);
int cfCallFunction(char* name, int nargs, ...);

int cfGetReturnInteger();
float cfGetReturnFloat();
int cfGetArgumentCount();
int cfGetIntegerArgument(int num);
float cfGetFloatArgument(int num);
int cfGetReturnAddress();
void cfReturnInteger(int v);
void cfReturnFloat(float v);

char* cfGetError();
void cfPrintInstructions();
void cfPrintStack();