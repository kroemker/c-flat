#pragma once

#include "symbols.h"
#include <cstdlib>

namespace cflat
{

	class Token
	{
	public:
		Token(int t);
		virtual ~Token(void) {};
		void				setType(int t) { type = t; }
		int					getType() { return type; }
		void				setLine(int l) { line = l; }
		int					getLine() { return line; }
		virtual const char*	toString();
		bool				isAssignToken();
	private:
		int type;
		int line;
	};

}