#pragma once

#include "token.h"
#include "symbols.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace cflat
{

	class Identifier : public Token
	{
	public:
		Identifier(char c);
		virtual ~Identifier(void);
		void				append(char n);
		char*				get() { return buffer; }
		void				checkKeyword();
		virtual const char*	toString() { return static_cast<const char*>(get()); }
	private:
		char*	buffer;
		int		position;
	};

}