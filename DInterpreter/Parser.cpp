#include "Parser.h"

#define PLACEHOLDER_STACK_START		-200
#define FUNCTION_END_LABEL			-3000

#define PUSH_PLACEHOLDER()			{ placeholderStack -= STACK_ENTRY_SIZE; }
#define RESET_PLACEHOLDER_STACK()	{ placeholderStack = PLACEHOLDER_STACK_START; }
#define RESET_STACK_LOCAL()			{ localStackSize = maxLocalStackSize = 0; }
#define PUSH_RETURN_VALUE(name)		{ name = maxReturnValues; maxReturnValues++; }
#define PUSH_GLOBAL()				{ globalStackSize += STACK_ENTRY_SIZE; }
#define PUSH_LOCAL()				{ localStackSize += STACK_ENTRY_SIZE; maxLocalStackSize = localStackSize > maxLocalStackSize ? localStackSize : maxLocalStackSize; }
#define PUSH(name)					{ name = localStackSize; PUSH_LOCAL(); }
#define POP()						{ localStackSize -= STACK_ENTRY_SIZE; }

namespace cflat
{

	Parser::Parser(Lexer* lexer) : lexer(lexer), localStackSize(0), maxLocalStackSize(0), placeholderStack(PLACEHOLDER_STACK_START)
	{
		if (!lexer->isPrelexed())
			lexer->prelex();
	}

	Parser::~Parser(void)
	{
	}

	Exception* Parser::parsenext()
	{
		try
		{
			if (decl(true) == DataType::NO_TYPE)
				throw exceptions::DECLARATION_EXPECTED;
		}
		catch (exceptions i)
		{
			Exception* e = new Exception(i, lexer->getExpressionCount(), lexer->look());
			lexer->next();
			return e;
		}
		return NULL;
	}

	void Parser::stmts()
	{
		Token* t0 = lexer->look();
		if (t0->gettype() == ';')
		{
			lexer->next();
			instructions.push_back(Instruction(Opcodes::NOP, 0, 0, 0));
		}
		else if (t0->gettype() == '{')
		{
			lexer->next();
			while (lexer->look()->gettype() != '}')
				stmts();
			lexer->next();
		}
		else if (t0->gettype() == Keywords::WHILE)
		{
			lexer->next();
			int stackSlot;
			PUSH(stackSlot);
			int start = instructions.size();
			boolexpr(stackSlot);
			int jid = instructions.size();
			instructions.push_back(Instruction(Opcodes::JF, 0, 0, 0));
			stmts();
			instructions.push_back(Instruction(Opcodes::J, start, 0, 0));
			instructions[jid].set(Opcodes::JF, Instruction::Arg(stackSlot), Instruction::Arg((int)instructions.size()), 0);
			POP();
		}
		else if (t0->gettype() == Keywords::IF)
		{
			lexer->next();
			int stackSlot;
			PUSH(stackSlot);
			boolexpr(stackSlot);
			int jid = instructions.size();
			instructions.push_back(Instruction(Opcodes::JF, 0, 0, 0));
			stmts();
			if (lexer->look()->gettype() == Keywords::ELSE)
			{
				int eid = instructions.size();
				instructions.push_back(Instruction(Opcodes::J, 0, 0, 0));
				instructions[jid].set(Opcodes::JF, Instruction::Arg(stackSlot), Instruction::Arg((int)instructions.size()), 0);
				lexer->next();
				stmts();
				instructions[eid].set(Opcodes::J, Instruction::Arg((int)instructions.size()), 0, 0);
			}
			else
				instructions[jid].set(Opcodes::JF, Instruction::Arg(stackSlot), Instruction::Arg((int)instructions.size()), 0);
			POP();
		}
		else if (t0->gettype() == TokenTypes::ID)
		{
			lexer->next();
			char* ident = dynamic_cast<Identifier*>(t0)->get();
			Token* t1 = lexer->look();
			if (t1->gettype() == '=')
			{
				Variable* v = getVariable(ident);
				if (v == NULL)
					throw exceptions::UNDEFINED_SYMBOL;
				lexer->next();
				DataType t = boolexpr(v->getstackslot());
				forcedTypecast(v->gettype(), t, v->getstackslot());
			}
			else if (t1->gettype() == '(')
			{
				functionCall(ident);
			}
			else
				throw exceptions::SYNTAX_ERROR;

			if (lexer->look()->gettype() != ';')
				throw exceptions::SEMICOLON_EXPECTED;
			lexer->next();
		}
		else if (t0->gettype() == Keywords::RETURN)
		{
			lexer->next();
			boolexpr(PLACEHOLDER_STACK_START);
			instructions.push_back(Instruction(Opcodes::J, Instruction::Arg(FUNCTION_END_LABEL), 0, 0));

			if (lexer->look()->gettype() != ';')
				throw exceptions::SEMICOLON_EXPECTED;
			lexer->next();

		}
		else if (decl(false) == DataType::NO_TYPE)
			throw exceptions::INVALID_CHARACTER;
	}

	DataType Parser::boolexpr(int stackSlot)
	{
		DataType t = joinexpr(stackSlot);
		if (lexer->look()->gettype() == Keywords::OR)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = joinexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LOR,
				Instruction::Arg(stackSlot),
				Instruction::Arg(stackSlot),
				Instruction::Arg(newSlot)));

			return DataType::S32;
		}
		return t;
	}

	DataType Parser::joinexpr(int stackSlot)
	{
		DataType t = equalityexpr(stackSlot);
		if (lexer->look()->gettype() == Keywords::AND)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = equalityexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LAND,
				Instruction::Arg(stackSlot),
				Instruction::Arg(stackSlot),
				Instruction::Arg(newSlot)));

			return DataType::S32;
		}
		return t;
	}

	DataType Parser::equalityexpr(int stackSlot)
	{
		DataType t = relexpr(stackSlot);
		if (lexer->look()->gettype() == TokenTypes::EQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = relexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::EQF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::EQ,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
		}
		else if (lexer->look()->gettype() == TokenTypes::UNEQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = relexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::EQF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));

				instructions.push_back(Instruction(Opcodes::LNOT,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot), 0));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::EQ,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));

				instructions.push_back(Instruction(Opcodes::LNOT,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot), 0));
				return DataType::S32;
			}
		}
		return t;
	}

	DataType Parser::relexpr(int stackSlot)
	{
		DataType t = mathexpr(stackSlot);
		if (lexer->look()->gettype() == '<')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = mathexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LTF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LT,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
		}
		else if (lexer->look()->gettype() == TokenTypes::LESS_EQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = mathexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LETF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LET,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot)));
				return DataType::S32;
			}
		}
		else if (lexer->look()->gettype() == '>')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = mathexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LTF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot),
					Instruction::Arg(stackSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LT,
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot),
					Instruction::Arg(stackSlot)));
				return DataType::S32;
			}

		}
		else if (lexer->look()->gettype() == TokenTypes::GREATER_EQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = mathexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LETF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot),
					Instruction::Arg(stackSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LET,
					Instruction::Arg(stackSlot),
					Instruction::Arg(newSlot),
					Instruction::Arg(stackSlot)));
				return DataType::S32;
			}
		}
		return t;
	}

	DataType Parser::mathexpr(int stackSlot)
	{
		DataType t = mathterm(stackSlot);
		while ((lexer->look()->gettype() == '+') || (lexer->look()->gettype() == '-'))
		{
			if (lexer->look()->gettype() == '+')
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType s = mathterm(newSlot);
				POP();

				if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
				{
					instructions.push_back(Instruction(Opcodes::ADDF,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::ADD,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::S32;
				}

			}
			else if (lexer->look()->gettype() == '-')
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType s = mathterm(newSlot);
				POP();

				if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
				{
					instructions.push_back(Instruction(Opcodes::SUBF,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::SUB,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::S32;
				}

			}
		}
		return t;
	}

	DataType Parser::mathterm(int stackSlot)
	{
		DataType t = mathunary(stackSlot);
		while ((lexer->look()->gettype() == '*') || (lexer->look()->gettype() == '/'))
		{
			if (lexer->look()->gettype() == '*')
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType s = mathunary(newSlot);
				POP();

				if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
				{
					instructions.push_back(Instruction(Opcodes::MULTF,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::MULT,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::S32;
				}

			}
			else if (lexer->look()->gettype() == '/')
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType s = mathunary(newSlot);
				POP();

				if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
				{
					instructions.push_back(Instruction(Opcodes::DIVF,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::DIV,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					t = DataType::S32;
				}
			}
		}
		return t;
	}

	DataType Parser::mathunary(int stackSlot)
	{
		DataType t;
		if (lexer->look()->gettype() == '-')
		{
			lexer->next();
			t = mathunary(stackSlot);

			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(stackSlot + STACK_ENTRY_SIZE), 0, 0));

			if (t == DataType::F32)
				instructions.push_back(Instruction(Opcodes::SUBF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot + STACK_ENTRY_SIZE),
					Instruction::Arg(stackSlot)));
			else
				instructions.push_back(Instruction(Opcodes::SUB,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot + STACK_ENTRY_SIZE),
					Instruction::Arg(stackSlot)));
		}
		else if (lexer->look()->gettype() == Keywords::NOT)
		{
			lexer->next();
			t = mathunary(stackSlot);
			instructions.push_back(Instruction(Opcodes::LNOT,
				Instruction::Arg(stackSlot), 0, 0));
		}
		else
			t = factor(stackSlot);

		return t;
	}

	DataType Parser::factor(int stackSlot)
	{
		switch (lexer->look()->gettype())
		{
		case '(':
		{
			lexer->next();
			DataType t = boolexpr(stackSlot);

			if (lexer->look()->gettype() == ')')
				lexer->next();
			else
				throw exceptions::CLOSEBRACKET_EXPECTED;
			return t;
		}
		case TokenTypes::INT_LITERAL:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(stackSlot),
				Instruction::Arg(dynamic_cast<Integer*>(lexer->look())->getvalue()), 0));
			lexer->next();
			return DataType::S32;
		}
		case TokenTypes::FLOAT_LITERAL:
		{
			instructions.push_back(Instruction(Opcodes::LDF,
				Instruction::Arg(stackSlot),
				Instruction::Arg(dynamic_cast<Float*>(lexer->look())->getvalue()), 0));
			lexer->next();
			return DataType::F32;
		}
		case Keywords::TRUE:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(stackSlot),
				Instruction::Arg(~0), 0));
			lexer->next();
			return DataType::S32;
		}
		case Keywords::FALSE:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(stackSlot),
				Instruction::Arg(0), 0));

			lexer->next();
			return DataType::S32;
		}
		case TokenTypes::ID:
		{
			char* ident = dynamic_cast<Identifier*>(lexer->look())->get();
			lexer->next();
			if (lexer->look()->gettype() == '(')
			{
				int retSlot = functionCall(ident);
				instructions.push_back(Instruction(Opcodes::MW,
					Instruction::Arg(stackSlot),
					Instruction::Arg(retSlot), 0));
				return DataType::S32; // FIXME: make functions typed
			}
			else
			{
				Variable* v = getVariable(ident);
				if (v)
				{
					if (typesizes[v->gettype()] == 1)
						instructions.push_back(Instruction(Opcodes::MB,
							Instruction::Arg(stackSlot),
							Instruction::Arg(v->getstackslot()), 0));
					else if (typesizes[v->gettype()] == 2)
						instructions.push_back(Instruction(Opcodes::MH,
							Instruction::Arg(stackSlot),
							Instruction::Arg(v->getstackslot()), 0));
					else if (typesizes[v->gettype()] == 4)
						instructions.push_back(Instruction(Opcodes::MW,
							Instruction::Arg(stackSlot),
							Instruction::Arg(v->getstackslot()), 0));
				}
				else
					throw exceptions::UNDEFINED_SYMBOL;
				return v->gettype();
			}
		}
		}
		throw exceptions::UNDEFINED_SYMBOL;
	}

	int Parser::functionCall(char* name)
	{
		Function* f = getFunction(name);
		if (!f)
			throw exceptions::UNDEFINED_SYMBOL;

		// push return value space /// FIXME: for now only 1 return value
		instructions.push_back(Instruction(Opcodes::PUSH, Instruction::Arg(STACK_ENTRY_SIZE), 0, 0));

		// push argument space
		int nargs = 0;
		int pid = instructions.size();

		lexer->next();
		while (lexer->look()->gettype() != ')')
		{
			if (nargs > 0)
			{
				if (lexer->look()->gettype() != ',')
					throw exceptions::COMMA_EXPECTED;
				lexer->next();
			}
			nargs++;
			boolexpr(-nargs * STACK_ENTRY_SIZE);
		}
		
		instructions.push_back(Instruction(Opcodes::LDI, Instruction::Arg(-(nargs + 1) * STACK_ENTRY_SIZE), Instruction::Arg(nargs), 0));

		instructions.push_back(Instruction(Opcodes::PUSH, Instruction::Arg((nargs + 1) * STACK_ENTRY_SIZE), 0, 0));

		if (f->isExternal())
			instructions.push_back(Instruction(Opcodes::CLE, Instruction::Arg(f->getAddress()), 0, 0));
		else
			instructions.push_back(Instruction(Opcodes::CL, Instruction::Arg(f->getAddress()), 0, 0));

		instructions.push_back(Instruction(Opcodes::POP, Instruction::Arg((nargs + 1) * STACK_ENTRY_SIZE), 0, 0));

		instructions.push_back(Instruction(Opcodes::POP, Instruction::Arg(STACK_ENTRY_SIZE), 0, 0));

		lexer->next();

		return -4;
	}

	DataType Parser::generalizedTypecast(DataType t, DataType s, int tSlot, int sSlot)
	{
		if (t == DataType::F32 && s != DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTF,
				Instruction::Arg(sSlot),
				Instruction::Arg(sSlot), 0));
			return DataType::F32;
		}
		else if (t != DataType::F32 && s == DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTF,
				Instruction::Arg(tSlot),
				Instruction::Arg(tSlot), 0));
			return DataType::F32;
		}
		return t;
	}

	void Parser::forcedTypecast(DataType t, DataType s, int tSlot)
	{
		if (t == s)
			return;

		if (t == DataType::F32 && s != DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTF,
				Instruction::Arg(tSlot),
				Instruction::Arg(tSlot), 0));
		}
		else if (t != DataType::F32 && s == DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTI,
				Instruction::Arg(tSlot),
				Instruction::Arg(tSlot), 0));
		}
		else if (t == DataType::S8 || t == DataType::U8)
		{
			int immValSlot;
			PUSH(immValSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(immValSlot),
				Instruction::Arg(0xFF), 0));
			instructions.push_back(Instruction(Opcodes::BAND,
				Instruction::Arg(tSlot),
				Instruction::Arg(tSlot),
				Instruction::Arg(immValSlot)));
		}
		else if (t == DataType::S16 || t == DataType::U16)
		{
			int immValSlot;
			PUSH(immValSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LDI,
				Instruction::Arg(immValSlot),
				Instruction::Arg(0xFFFF), 0));
			instructions.push_back(Instruction(Opcodes::BAND,
				Instruction::Arg(tSlot),
				Instruction::Arg(tSlot),
				Instruction::Arg(immValSlot)));
		}
	}

	void Parser::decls(bool global)
	{
		DataType declType;
		while ((declType = decl(global)) != NO_TYPE)
		{
			lexer->next();
		}
	}

	DataType Parser::datatype()
	{
		DataType varType = DataType::NO_TYPE;
		Token* t1 = lexer->look();
		if (t1->gettype() == Keywords::T_U8)
			varType = DataType::U8;
		else if (t1->gettype() == Keywords::T_U16)
			varType = DataType::U16;
		else if (t1->gettype() == Keywords::T_U32)
			varType = DataType::U32;
		else if (t1->gettype() == Keywords::T_S8)
			varType = DataType::S8;
		else if (t1->gettype() == Keywords::T_S16)
			varType = DataType::S16;
		else if (t1->gettype() == Keywords::T_S32 || t1->gettype() == Keywords::INT)
			varType = DataType::S32;
		else if (t1->gettype() == Keywords::T_F32 || t1->gettype() == Keywords::FLOAT)
			varType = DataType::F32;
		else if (t1->gettype() == Keywords::FUNCTION)
			varType = DataType::FUNC;

		if (varType != DataType::NO_TYPE)
			lexer->next();

		return varType;
	}

	DataType Parser::decl(bool global)
	{
		DataType varType = datatype();
		if (varType != DataType::NO_TYPE)
		{
			Token* t2 = lexer->look();
			if (t2->gettype() != TokenTypes::ID)
				throw exceptions::ID_EXPECTED;

			char* ident = dynamic_cast<Identifier*>(t2)->get();
			Variable* v = getVariable(ident);
			if (v)
				throw exceptions::SYMBOL_REDEFINITION;

			if (varType == DataType::FUNC)
			{
				int functionStart = instructions.size();
				int nargs = 0;

				lexer->next();
				if (lexer->look()->gettype() != '(')
					throw exceptions::OPENBRACKET_EXPECTED;

				// push return value placeholder
				PUSH_PLACEHOLDER();

				// parse arguments
				lexer->next();
				while (lexer->look()->gettype() != ')')
				{
					if (nargs > 0)
					{
						if (lexer->look()->gettype() != ',')
							throw COMMA_EXPECTED;
						lexer->next();
					}

					DataType argType = datatype();
					if (argType == DataType::NO_TYPE)
						throw exceptions::TYPE_EXPECTED;

					Token* argNameId = lexer->look();
					if (argNameId->gettype() != TokenTypes::ID)
						throw exceptions::ID_EXPECTED;

					variables.push_back(Variable(argType, dynamic_cast<Identifier*>(argNameId)->get(),
						placeholderStack, false));
					PUSH_PLACEHOLDER();
					nargs++;

					lexer->next();
				}
				lexer->next();

				// push arg count
				variables.push_back(Variable(DataType::U32, "__argc",
					placeholderStack, false));
				PUSH_PLACEHOLDER();

				// push return address (don't push on placeholder stack, since it is the last element)
				variables.push_back(Variable(DataType::U32, "__ra",
					placeholderStack, false));

				functions.push_back(Function(ident, functionStart, false));
				instructions.push_back(Instruction(Opcodes::PUSH, 0, 0, 0));

				// do function
				stmts();

				// update stack size
				instructions[functionStart].set(Opcodes::PUSH, Instruction::Arg(maxLocalStackSize), 0, 0);
				int functionEnd = instructions.size();
				instructions.push_back(Instruction(Opcodes::POP, Instruction::Arg(maxLocalStackSize), 0, 0));
				instructions.push_back(Instruction(Opcodes::JR, 0, 0, 0));

				// replace argument addresses
				for (int i = functionStart; i < instructions.size(); i++)
				{
					instructions[i].updateJumpTarget(FUNCTION_END_LABEL, functionEnd);
					instructions[i].translatePlaceholder(PLACEHOLDER_STACK_START, -placeholderStack + maxLocalStackSize);
				}

				// reset stack and remove globals
				RESET_STACK_LOCAL();
				RESET_PLACEHOLDER_STACK();
				removeAllNonGlobals();
			}
			else
			{
				if (global)
				{
					variables.push_back(Variable(varType, ident, globalStackSize, true));
					PUSH_GLOBAL();
				}
				else
				{
					variables.push_back(Variable(varType, ident, localStackSize, false));
					PUSH_LOCAL();
				}
				lexer->next();
				if (lexer->look()->gettype() != ';')
					throw exceptions::SEMICOLON_EXPECTED;
				lexer->next();
			}
		}
		return varType;
	}

	void Parser::removeAllNonGlobals()
	{
		auto it = variables.begin();
		while (it != variables.end())
		{
			if (!it->isglobal())
				it = variables.erase(it);
			else
				++it;
		}
	}

	Variable* Parser::getVariable(char* name)
	{
		for (int i = 0; i < variables.size(); i++)
		{
			if (strcmp(name, variables[i].getname()) == 0)
				return &variables[i];
		}
		return NULL;
	}

	Function* Parser::getFunction(char* name)
	{
		for (int i = 0; i < functions.size(); i++)
		{
			if (strcmp(name, functions[i].getName()) == 0)
				return &functions[i];
		}
		return NULL;
	}

}