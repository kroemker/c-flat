#include "Instruction.h"

#include <iostream>
#include <cassert>

cflat::Instruction::Instruction(int opcode, Argument a0, Argument a1, Argument a2) : opcode(opcode), a0(a0), a1(a1), a2(a2)
{
}

cflat::Instruction::~Instruction()
{
}

void cflat::Instruction::set(int opcode, Argument a0, Argument a1, Argument a2)
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

void cflat::Instruction::execute(Stack* stackPtr)
{
	Stack stack = NULL;
	if (stackPtr)
		stack = *stackPtr;

	switch (opcode)
	{
	case Opcodes::ADD:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i + STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::ADDF:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).f = STACK(a1.i).f + STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::BAND:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i & STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::BNOT:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = ~STACK(a1.i).i;
		PC++;
		break;
	case Opcodes::BOR:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i | STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::BXOR:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i ^ STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::CL:
		assert(STACK_INBOUNDS(0));
		STACK(0).i = PC + 1;
		PC = a0.i;
		break;
	case Opcodes::CLE:
		ExternalFunctionPtr f;
		if (sizeof(ExternalFunctionPtr) == 4)
			f = (ExternalFunctionPtr)(a0.i);
		else if (sizeof(ExternalFunctionPtr) == 8)
			f = (ExternalFunctionPtr)((a0.i << 32) | a1.i);
		else
			throw exceptions::NOT_IMPLEMENTED;
		f(stack);
		PC++;
		break;
	case Opcodes::CTF:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).f = (float)STACK(a1.i).i;
		PC++;
		break;
	case Opcodes::CTI:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = (int)STACK(a1.i).f;
		PC++;
		break;
	case Opcodes::DIV:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i / STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::DIVF:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).f = STACK(a1.i).f / STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::EQ:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).i == STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::EQF:
		assert(STACK_INBOUNDS(a0.i));
		STACK(a0.i).i = STACK(a1.i).f == STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::INIT:
		*stackPtr = new StackEntry[STACK_NUM_ENTRIES];
		stack = *stackPtr;
		SP = STACK_SIZE;
		PC = 0;
		PC++;
		break;
	case Opcodes::J:
		PC = a0.i;
		break;
	case Opcodes::JF:
		if (!STACK(a0.i).i)
			PC = a1.i;
		else
			PC++;
		break;
	case Opcodes::JR:
		assert(STACK_INBOUNDS(0));
		PC = STACK(0).i;
		break;
	case Opcodes::JT:
		if (STACK(a0.i).i)
			PC = a1.i;
		else
			PC++;
		break;
	case Opcodes::LAND:
		STACK(a0.i).i = STACK(a1.i).i && STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::LDF:
		STACK(a0.i).f = a1.f;
		PC++;
		break;
	case Opcodes::LDI:
		STACK(a0.i).i = a1.i;
		PC++;
		break;
	case Opcodes::LET:
		STACK(a0.i).i = STACK(a1.i).i <= STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::LETF:
		STACK(a0.i).i = STACK(a1.i).f <= STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::LNOT:
		STACK(a0.i).i = !STACK(a1.i).i;
		PC++;
		break;
	case Opcodes::LOR:
		STACK(a0.i).i = STACK(a1.i).i || STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::LT:
		STACK(a0.i).i = STACK(a1.i).i < STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::LTF:
		STACK(a0.i).i = STACK(a1.i).f < STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::MB:
	case Opcodes::MH:
	case Opcodes::MW:
		if (a2.i == 0)
			STACK(a0.i).i = STACK(a1.i).i;
		else
			STACK(a0.i).i = GLOBAL(a1.i).i;
		PC++;
		break;
	case Opcodes::MG:
		if (a2.i == 0)
			GLOBAL(a0.i).i = STACK(a1.i).i;
		else
			GLOBAL(a0.i).i = GLOBAL(a1.i).i;
		PC++;
		break;
	case Opcodes::MOD:
		STACK(a0.i).i = STACK(a1.i).i % STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::MULT:
		STACK(a0.i).i = STACK(a1.i).i * STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::MULTF:
		STACK(a0.i).f = STACK(a1.i).f * STACK(a2.i).f;
		PC++;
		break;
	case Opcodes::NOP:
		PC++;
		break;
	case Opcodes::POP:
		SP += a0.i;
		PC++;
		break;
	case Opcodes::PUSH:
		SP -= a0.i;
		PC++;
		break;
	case Opcodes::Q:
		delete[] (*stackPtr);
		*stackPtr = NULL;
		break;
	case Opcodes::SUB:
		STACK(a0.i).i = STACK(a1.i).i - STACK(a2.i).i;
		PC++;
		break;
	case Opcodes::SUBF:
		STACK(a0.i).f = STACK(a1.i).f - STACK(a2.i).f;
		PC++;
		break;
	}
}
