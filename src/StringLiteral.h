#pragma once

#include "Token.h"
#include "Symbols.h"

namespace cflat
{

	class StringLiteral : public Token
	{
	public:
		StringLiteral(void);
		virtual ~StringLiteral(void);
		void				append(char n);
		char*				get() { return buffer; }
		virtual const char*	toString() { return (const char*)get(); }
	private:
		char*	buffer;
		int		position;
	};

}