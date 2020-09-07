#pragma once

#include "token.h"
#include "symbols.h"
#include <string>

namespace cflat
{

	class Float : public Token
	{
	public:
		Float(float v);
		virtual ~Float(void) {};
		void				setvalue(float v) { value = v; }
		float				getvalue() { return value; }
		virtual const char*	tostring();
	private:
		float value;
	};

}