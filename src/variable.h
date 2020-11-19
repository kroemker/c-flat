#pragma once

#include "Symbols.h"

namespace cflat
{

	class Variable
	{
	public:
		Variable(DataType type, char* name, int stackSlot, bool global);
		virtual ~Variable(void);
		DataType	getType() { return type; }
		char*		getName() { return name; }
		int			getStackSlot() { return stackSlot; }
		bool		isglobal() { return global; }
	private:
		char*		name;
		DataType	type;
		int			stackSlot;
		bool		global;
	};

}