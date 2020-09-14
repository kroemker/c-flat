#include "function.h"

namespace cflat
{
    Function::Function(char* name, int address) : name(name), address(address), external(false) 
    {
    }

	Function::Function(char* name, ExternalFunctionPtr functionPtr) : name(name), functionPtr(functionPtr), external(true)
    {
    }

}