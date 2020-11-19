#include "Function.h"

namespace cflat
{
	Function::Function(char* name, DataType type, int address) : name(name), address(address), external(false), type(type)
    {
    }

	Function::Function(char* name, DataType type, ExternalFunctionPtr functionPtr) : name(name), functionPtr(functionPtr), external(true), type(type)
    {
    }

}