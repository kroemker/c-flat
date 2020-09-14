#pragma once

#include <cstring>
#include "symbols.h"

namespace cflat
{

	class Instruction
	{
	public:

		Instruction(int opcode, Argument a0, Argument a1, Argument a2);
		~Instruction();
		void set(int opcode, Argument a0, Argument a1, Argument a2);
		void print();
		void translatePlaceholder(int placeholderStackStart, int translation);
		void updateJumpTarget(int before, int after);
		void execute(Stack* stack);
	private:
		int opcode;
		Argument a0, a1, a2;
	};

}