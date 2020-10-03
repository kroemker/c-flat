#include "Optimizer.h"

namespace cflat {

	Optimizer::Optimizer(Parser* parser) : parser(parser)
	{
	}

	Optimizer::~Optimizer()
	{
	}

	void Optimizer::optimize() 
	{
		// first pass: set unnecessary instructions to nops
		for (int i = 0; i < parser->instructions.size(); i++)
		{
			Instruction* instr = &parser->instructions[i];
			Instruction* next = (i + 1 < parser->instructions.size()) ? &parser->instructions[i + 1] : NULL;

			bool nopEquivalent = false;

			switch (instr->getOpcode())
			{
			case Opcodes::J:
				nopEquivalent = instr->getArgument(0).i == i + 1;
				if (next)
					nopEquivalent = nopEquivalent || next->Equals(*instr);
				break;
			case Opcodes::JF:
			case Opcodes::JT:
				nopEquivalent = instr->getArgument(1).i == i + 1;
				if (next)
					nopEquivalent = nopEquivalent || next->Equals(*instr);
				break;
			case Opcodes::PUSH:
			case Opcodes::POP:
				nopEquivalent = instr->getArgument(0).i == 0;
				break;
			default:
				break;
			}

			if (nopEquivalent)
				instr->set(Opcodes::NOP, 0, 0, 0);
		}

		// second pass: remove nops and update jumps
		for (int i = 0; i < parser->instructions.size(); i++)
		{
			if (parser->instructions[i].getOpcode() == Opcodes::NOP)
			{
				// update jumps
				for (int j = 0; j < parser->instructions.size(); j++)
				{
					Instruction* instr = &parser->instructions[j];
					switch (instr->getOpcode())
					{
					case Opcodes::J:
					case Opcodes::CL:
						if (instr->getArgument(0).i > i)
							instr->set(instr->getOpcode(), Argument(instr->getArgument(0).i - 1), 0, 0);
						break;
					case Opcodes::JF:
					case Opcodes::JT:
						if (instr->getArgument(1).i > i)
							instr->set(instr->getOpcode(), instr->getArgument(0), Argument(instr->getArgument(1).i - 1), 0);
						break;
					default:
						break;
					}
				}

				// remove instruction
				parser->instructions.erase(parser->instructions.begin() + i);
				i--;
			}
		}
	}
}