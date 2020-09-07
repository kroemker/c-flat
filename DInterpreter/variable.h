#pragma once

#include "symbols.h"

namespace cflat
{

	class Variable
	{
	public:
		Variable(DataType type, char* name, int stackSlot, bool global);
		virtual ~Variable(void);
		DataType	gettype() { return type; }
		char*	getname() { return name; }
		int		getstackslot() { return stackSlot; }
		bool	isglobal() { return global; }
	private:
		char*		name;
		DataType	type;
		int			stackSlot;
		bool		global;
	};

}