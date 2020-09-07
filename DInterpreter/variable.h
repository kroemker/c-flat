#pragma once

#include "symbols.h"

namespace cflat
{

	class Variable
	{
	public:
		Variable(DataType type, char* name, int stackSlot);
		virtual ~Variable(void);
		DataType	gettype() { return type; }
		char*	getname() { return name; }
		int		getstackslot() { return stackSlot; }
	private:
		char*	 name;
		DataType	 type;
		int		 stackSlot;
	};

}