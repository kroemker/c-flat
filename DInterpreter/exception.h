#pragma once

#include "symbols.h"

namespace cflat
{

	class Exception
	{
	public:
		Exception(int type, int exprn, int line) { this->type = type; exprnum = exprn; this->line = line; }
		virtual		~Exception(void) {};
		int			gettype() { return type; }
		int			getline() { return line; }
		int			getexprnum() { return exprnum; }
		const char*	tostring() { return texceptions_s[type]; }
	private:
		int type;
		int exprnum;
		int line;
	};

}