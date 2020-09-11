#pragma once

#include <cstring>
#include "symbols.h"

namespace cflat
{
	class Instruction
	{
	public:
		union Arg
		{
			int i;
			float f;
			void* ptr;

			Arg() { memset(this, 0, sizeof(Arg)); }
			Arg(int i) { this->i = i; }
			Arg(float f) { this->f = f; }
			Arg(void* ptr) { this->ptr = ptr; }
		};

		Instruction(int opcode, Arg a0, Arg a1, Arg a2);
		~Instruction();
		void set(int opcode, Arg a0, Arg a1, Arg a2);
		void print();
		void translatePlaceholder(int placeholderStackStart, int translation);
		void updateJumpTarget(int before, int after);
		//execute(Stack &s);
	private:
		int opcode;
		Arg a0, a1, a2;
	};

}