#pragma once

#include "token.h"
#include "symbols.h"
#include <cstring>
#include <cstdio>

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