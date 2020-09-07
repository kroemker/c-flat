#include "variable.h"

namespace cflat
{
	Variable::Variable(DataType type, char* name, int stackSlot) : name(name), type(type), stackSlot(stackSlot)
	{
	}

	Variable::~Variable(void)
	{
	}
}