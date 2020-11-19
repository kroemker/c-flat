#pragma once

#include <cstring>
#include <cstdio>

#include "Token.h"
#include "Symbols.h"

namespace cflat
{

	class Float : public Token
	{
	public:
		Float(float v);
		virtual ~Float(void) {};
		void				setValue(float v) { value = v; }
		float				getValue() { return value; }
		virtual const char*	toString();
	private:
		float value;
	};

}