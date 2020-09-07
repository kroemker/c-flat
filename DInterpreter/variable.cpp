#include "variable.h"

namespace cflat
{
	Variable::Variable(DataType type, char* name, int stackSlot, bool global) 
		: name(name), type(type), stackSlot(stackSlot), global(global)
	{
	}

	Variable::~Variable(void)
	{
	}
}