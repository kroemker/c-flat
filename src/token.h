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
		void				settype(int t) { type = t; }
		int					gettype() { return type; }
		void				setline(int l) { line = l; }
		int					getline() { return line; }
		virtual const char*	tostring();
	private:
		int type;
		int line;
	};

}