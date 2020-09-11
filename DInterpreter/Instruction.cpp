#include "Instruction.h"

#include <iostream>

cflat::Instruction::Instruction(int opcode, Arg a0, Arg a1, Arg a2) : opcode(opcode), a0(a0), a1(a1), a2(a2)
{
}

cflat::Instruction::~Instruction()
{
}

void cflat::Instruction::set(int opcode, Arg a0, Arg a1, Arg a2)
{
	this->opcode = opcode;
	this->a0 = a0;
	this->a1 = a1;
	this->a2 = a2;
}

void cflat::Instruction::print()
{
	std::cout << opcodes_s[opcode];
	if (opcodes_nargs[opcode] > 0)
		std::cout << "\t" << a0.i;
	if (opcodes_nargs[opcode] > 1)
		std::cout << ", " << a1.i;
	if (opcodes_nargs[opcode] > 2)
		std::cout << ", " << a2.i;
	std::cout << "\n";
}

void cflat::Instruction::translatePlaceholder(int placeholderStackStart, int translation)
{
	if (opcode == Opcodes::PUSH || opcode == Opcodes::POP || opcode == Opcodes::CL || opcode == Opcodes::CLE)
		return;

	if (a0.i <= placeholderStackStart)
		a0.i += translation;

	if (opcode == Opcodes::LDI || opcode == Opcodes::LDF)
		return;

	if (a1.i <= placeholderStackStart)
		a1.i += translation;

	if (a2.i <= placeholderStackStart)
		a2.i += translation;
}

void cflat::Instruction::updateJumpTarget(int before, int after)
{
	if (opcode == Opcodes::J && a0.i == before)
		a0 = after;
	
	if ((opcode == Opcodes::JT || opcode == Opcodes::JF) && a1.i == before)
		a1 = after;
}
