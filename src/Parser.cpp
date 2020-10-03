#include "Parser.h"

#define PLACEHOLDER_STACK_START		-200
#define FUNCTION_END_LABEL			-3000
#define INVALID_STACK_SLOT			-40000000

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

	Parser::Parser(Lexer* lexer) : lexer(lexer), globalStackSize(0), localStackSize(0), maxLocalStackSize(0), placeholderStack(PLACEHOLDER_STACK_START), error(false), entryPoint(NULL)
	{
	}

	Parser::~Parser(void)
	{
	}

	void Parser::registerExternalFunction(char * name, ExternalFunctionPtr f)
	{
		functions.push_back(Function(name, DataType::VOID, f));
	}

	int Parser::setEntryPoint(char* functionName)
	{
		Function* f = getFunction(functionName);
		if (!f || f->isExternal())
			return 0;

		entryPoint = f;

		if (instructions.size() >= 2)
			instructions[1].set(Opcodes::CL, Argument(entryPoint->getAddress()), 0, 0);

		return 1;
	}

	Exception* Parser::parsenext()
	{
		try
		{
			if (globaldecl() == DataType::NO_TYPE)
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

	void Parser::parse()
	{
		instructions.push_back(Instruction(Opcodes::PUSH, Argument(STACK_ENTRY_SIZE), 0, 0));
		instructions.push_back(Instruction(Opcodes::CL, 0, 0, 0));
		instructions.push_back(Instruction(Opcodes::POP, Argument(STACK_ENTRY_SIZE), 0, 0));
		instructions.push_back(Instruction(Opcodes::Q, 0, 0, 0));

		while (!lexer->isEndOfTokenList())
		{
			Exception* exc = parsenext();
			if (exc)
			{
				sprintf(errorString, "Line: %d, Expression: %d @ %s\nError: %s",
					exc->gettoken()->getline(),
					exc->getexprnum(),
					exc->gettoken()->tostring(),
					exc->tostring());
				error = true;
				break;
			}
		}
		// add yield instruction for library usage
		instructions.push_back(Instruction(Opcodes::YLD, 0, 0, 0));

		// handle entry point
		if (!entryPoint)
			setEntryPoint("main");
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
			assignexpr(stackSlot);
			int jid = instructions.size();
			instructions.push_back(Instruction(Opcodes::JF, 0, 0, 0));
			stmts();
			instructions.push_back(Instruction(Opcodes::J, start, 0, 0));
			instructions[jid].set(Opcodes::JF, Argument(stackSlot), Argument((int)instructions.size()), 0);
			POP();
		}
		else if (t0->gettype() == Keywords::IF)
		{
			lexer->next();
			int stackSlot;
			PUSH(stackSlot);
			assignexpr(stackSlot);
			int jid = instructions.size();
			instructions.push_back(Instruction(Opcodes::JF, 0, 0, 0));
			stmts();
			if (lexer->look()->gettype() == Keywords::ELSE)
			{
				int eid = instructions.size();
				instructions.push_back(Instruction(Opcodes::J, 0, 0, 0));
				instructions[jid].set(Opcodes::JF, Argument(stackSlot), Argument((int)instructions.size()), 0);
				lexer->next();
				stmts();
				instructions[eid].set(Opcodes::J, Argument((int)instructions.size()), 0, 0);
			}
			else
				instructions[jid].set(Opcodes::JF, Argument(stackSlot), Argument((int)instructions.size()), 0);
			POP();
		}
		else if (t0->gettype() == Keywords::RETURN)
		{
			lexer->next();
			assignexpr(PLACEHOLDER_STACK_START);
			instructions.push_back(Instruction(Opcodes::J, Argument(FUNCTION_END_LABEL), 0, 0));

			if (lexer->look()->gettype() != ';')
				throw exceptions::SEMICOLON_EXPECTED;
			lexer->next();
		}
		else
		{
			DataType t = datatype();

			if (t != DataType::NO_TYPE)
			{
				while (true)
				{
					Token* t1 = lexer->look();
					if (t1->gettype() != TokenTypes::ID)
						throw exceptions::ID_EXPECTED;

					char* ident = dynamic_cast<Identifier*>(t1)->get();
					int stackSlot = localStackSize;
					variables.push_back(Variable(t, ident, stackSlot, false));
					PUSH_LOCAL();

					if (lexer->lookahead(1)->gettype() == '=')
						assignexpr(stackSlot);
					else
						lexer->next();

					if (lexer->look()->gettype() == ',')
						lexer->next();
					else
						break;
				};

				if (lexer->look()->gettype() != ';')
					throw exceptions::SEMICOLON_EXPECTED;
				lexer->next();
			}
			else
			{
				int stackSlot;
				PUSH(stackSlot);
				assignexpr(stackSlot);
				POP();
				if (lexer->look()->gettype() != ';')
					throw exceptions::SEMICOLON_EXPECTED;
				lexer->next();
			}
		}
	}

	DataType Parser::assignexpr(int stackSlot)
	{
		DataType t = DataType::NO_TYPE;
		Token* t0 = lexer->look();

		if (t0->gettype() == TokenTypes::ID && lexer->lookahead(1)->gettype() == '=')
		{
			lexer->next(); // '='
			lexer->next();

			char* ident = dynamic_cast<Identifier*>(t0)->get();
			Variable* v = getVariable(ident);
			if (v == NULL)
				throw exceptions::UNDEFINED_SYMBOL;

			if (v->isglobal())
			{
				t = assignexpr(stackSlot);
				forcedTypecast(v->gettype(), t, stackSlot);
				instructions.push_back(Instruction(Opcodes::MG, Argument(v->getstackslot()), Argument(stackSlot), 0));
				t = v->gettype();
			}
			else
			{
				t = assignexpr(v->getstackslot());
				forcedTypecast(v->gettype(), t, v->getstackslot());
				t = v->gettype();
			}
		}
		else
			t = boolexpr(stackSlot);
		return t;
	}

	DataType Parser::boolexpr(int stackSlot)
	{
		DataType t = joinexpr(stackSlot);
		while (lexer->look()->gettype() == Keywords::OR)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = joinexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LOR,
				Argument(stackSlot),
				Argument(stackSlot),
				Argument(newSlot)));

			t = DataType::S32;
		}
		return t;
	}

	DataType Parser::joinexpr(int stackSlot)
	{
		DataType t = bitorexpr(stackSlot);
		while (lexer->look()->gettype() == Keywords::AND)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = bitorexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LAND,
				Argument(stackSlot),
				Argument(stackSlot),
				Argument(newSlot)));

			t = DataType::S32;
		}
		return t;
	}

	DataType Parser::bitorexpr(int stackSlot)
	{
		DataType t = bitxorexpr(stackSlot);
		while (lexer->look()->gettype() == '|')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = bitxorexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::BOR,
				Argument(stackSlot),
				Argument(stackSlot),
				Argument(newSlot)));

			t = DataType::S32;
		}
		return t;
	}

	DataType Parser::bitxorexpr(int stackSlot)
	{
		DataType t = bitandexpr(stackSlot);
		while (lexer->look()->gettype() == '^')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = bitandexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::BXOR,
				Argument(stackSlot),
				Argument(stackSlot),
				Argument(newSlot)));

			t = DataType::S32;
		}
		return t;
	}

	DataType Parser::bitandexpr(int stackSlot)
	{
		DataType t = equalityexpr(stackSlot);
		while (lexer->look()->gettype() == '&')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = equalityexpr(newSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::BAND,
				Argument(stackSlot),
				Argument(stackSlot),
				Argument(newSlot)));

			t = DataType::S32;
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
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::EQ,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
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
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));

				instructions.push_back(Instruction(Opcodes::LNOT,
					Argument(stackSlot),
					Argument(stackSlot), 0));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::EQ,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));

				instructions.push_back(Instruction(Opcodes::LNOT,
					Argument(stackSlot),
					Argument(stackSlot), 0));
				return DataType::S32;
			}
		}
		return t;
	}

	DataType Parser::relexpr(int stackSlot)
	{
		DataType t = shiftexpr(stackSlot);
		if (lexer->look()->gettype() == '<')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = shiftexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LTF,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LT,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				return DataType::S32;
			}
		}
		else if (lexer->look()->gettype() == TokenTypes::LESS_EQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = shiftexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LETF,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LET,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				return DataType::S32;
			}
		}
		else if (lexer->look()->gettype() == '>')
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = shiftexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LTF,
					Argument(stackSlot),
					Argument(newSlot),
					Argument(stackSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LT,
					Argument(stackSlot),
					Argument(newSlot),
					Argument(stackSlot)));
				return DataType::S32;
			}

		}
		else if (lexer->look()->gettype() == TokenTypes::GREATER_EQUALS)
		{
			lexer->next();
			int newSlot;
			PUSH(newSlot);
			DataType s = shiftexpr(newSlot);
			POP();

			if (generalizedTypecast(t, s, stackSlot, newSlot) == DataType::F32)
			{
				instructions.push_back(Instruction(Opcodes::LETF,
					Argument(stackSlot),
					Argument(newSlot),
					Argument(stackSlot)));
				return DataType::S32;
			}
			else
			{
				instructions.push_back(Instruction(Opcodes::LET,
					Argument(stackSlot),
					Argument(newSlot),
					Argument(stackSlot)));
				return DataType::S32;
			}
		}
		return t;
	}

	DataType Parser::shiftexpr(int stackSlot)
	{
		DataType t = mathexpr(stackSlot);
		while ((lexer->look()->gettype() == TokenTypes::SHIFT_LEFT) || (lexer->look()->gettype() == TokenTypes::SHIFT_RIGHT))
		{
			if (lexer->look()->gettype() == TokenTypes::SHIFT_LEFT)
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType t = mathexpr(newSlot);
				POP();

				instructions.push_back(Instruction(Opcodes::SL,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
			}
			else if (lexer->look()->gettype() == TokenTypes::SHIFT_RIGHT)
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType t = mathexpr(newSlot);
				POP();

				instructions.push_back(Instruction(Opcodes::SR,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
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
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::ADD,
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
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
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::SUB,
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::S32;
				}

			}
		}
		return t;
	}

	DataType Parser::mathterm(int stackSlot)
	{
		DataType t = mathunary(stackSlot);
		while ((lexer->look()->gettype() == '*') || (lexer->look()->gettype() == '/') || (lexer->look()->gettype() == '%'))
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
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::MULT,
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
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
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::DIV,
						Argument(stackSlot),
						Argument(stackSlot),
						Argument(newSlot)));
					t = DataType::S32;
				}
			}
			else if (lexer->look()->gettype() == '%')
			{
				lexer->next();
				int newSlot;
				PUSH(newSlot);
				DataType s = mathunary(newSlot);
				POP();


				instructions.push_back(Instruction(Opcodes::MOD,
					Argument(stackSlot),
					Argument(stackSlot),
					Argument(newSlot)));
				t = DataType::S32;
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
				Argument(stackSlot + STACK_ENTRY_SIZE), 0, 0));

			if (t == DataType::F32)
				instructions.push_back(Instruction(Opcodes::SUBF,
					Argument(stackSlot),
					Argument(stackSlot + STACK_ENTRY_SIZE),
					Argument(stackSlot)));
			else
				instructions.push_back(Instruction(Opcodes::SUB,
					Argument(stackSlot),
					Argument(stackSlot + STACK_ENTRY_SIZE),
					Argument(stackSlot)));
		}
		else if (lexer->look()->gettype() == Keywords::NOT)
		{
			lexer->next();
			t = mathunary(stackSlot);
			instructions.push_back(Instruction(Opcodes::LNOT,
				Argument(stackSlot), 0, 0));
		}
		else if (lexer->look()->gettype() == '~')
		{
			lexer->next();
			t = mathunary(stackSlot);
			instructions.push_back(Instruction(Opcodes::BNOT,
				Argument(stackSlot), 0, 0));
		}
		else if (lexer->look()->gettype() == TokenTypes::INCREMENT)
		{
			lexer->next();
			Variable* var;
			t = memexpr(stackSlot, &var);
			instructions.push_back(Instruction(Opcodes::INC,
				Argument(var->getstackslot()), Argument(var->isglobal()), 0));
			loadVarToStack(stackSlot, var);
		}
		else if (lexer->look()->gettype() == TokenTypes::DECREMENT)
		{
			lexer->next();
			Variable* var;
			t = memexpr(stackSlot, &var);
			instructions.push_back(Instruction(Opcodes::DEC,
				Argument(var->getstackslot()), Argument(var->isglobal()), 0));
			loadVarToStack(stackSlot, var);
		}
		else
			t = factor(stackSlot);

		return t;
	}

	DataType Parser::memexpr(int stackSlot, Variable** outvar)
	{
		Token* t0 = lexer->look();
		if (t0->gettype() != TokenTypes::ID)
			throw exceptions::ID_EXPECTED;

		char* ident = dynamic_cast<Identifier*>(t0)->get();
		Variable* v = getVariable(ident);
		if (!v)
			throw exceptions::UNDEFINED_SYMBOL;

		loadVarToStack(stackSlot, v);

		lexer->next();
		if (lexer->look()->gettype() == TokenTypes::INCREMENT)
		{
			lexer->next();
			instructions.push_back(Instruction(Opcodes::INC,
				Argument(v->getstackslot()), Argument(v->isglobal()), 0));
		}
		else if (lexer->look()->gettype() == TokenTypes::DECREMENT)
		{
			lexer->next();
			instructions.push_back(Instruction(Opcodes::DEC,
				Argument(v->getstackslot()), Argument(v->isglobal()), 0));
		}

		if (outvar)
			*outvar = v;
		return v->gettype();
	}

	DataType Parser::factor(int stackSlot)
	{
		switch (lexer->look()->gettype())
		{
		case '(':
		{
			lexer->next();
			DataType t = assignexpr(stackSlot);

			if (lexer->look()->gettype() == ')')
				lexer->next();
			else
				throw exceptions::CLOSEBRACKET_EXPECTED;
			return t;
		}
		case TokenTypes::INT_LITERAL:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Argument(stackSlot),
				Argument(dynamic_cast<Integer*>(lexer->look())->getvalue()), 0));
			lexer->next();
			return DataType::S32;
		}
		case TokenTypes::FLOAT_LITERAL:
		{
			instructions.push_back(Instruction(Opcodes::LDF,
				Argument(stackSlot),
				Argument(dynamic_cast<Float*>(lexer->look())->getvalue()), 0));
			lexer->next();
			return DataType::F32;
		}
		case Keywords::TRUE:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Argument(stackSlot),
				Argument(~0), 0));
			lexer->next();
			return DataType::S32;
		}
		case Keywords::FALSE:
		{
			instructions.push_back(Instruction(Opcodes::LDI,
				Argument(stackSlot),
				Argument(0), 0));

			lexer->next();
			return DataType::S32;
		}
		case TokenTypes::ID:
		{
			char* ident = dynamic_cast<Identifier*>(lexer->look())->get();
			if (lexer->lookahead(1)->gettype() == '(')
			{
				lexer->next();
				int retSlot = functionCall(ident);
				instructions.push_back(Instruction(Opcodes::MW,
					Argument(stackSlot),
					Argument(retSlot), 0));
				return getFunction(ident)->getType();
			}
			else
				return memexpr(stackSlot, NULL);
		}
		}
		throw exceptions::UNDEFINED_SYMBOL;
	}

	int Parser::functionCall(char* name)
	{
		Function* f = getFunction(name);
		if (!f)
			throw exceptions::UNDEFINED_SYMBOL;

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
			assignexpr(-(nargs + 1) * STACK_ENTRY_SIZE);
		}

		instructions.push_back(Instruction(Opcodes::LDI, Argument(-(nargs + 2) * STACK_ENTRY_SIZE), Argument(nargs), 0));

		// push return value space /// FIXME: for now only 1 return value
		// push return-value + n-arguments + nargs + ra space //
		instructions.push_back(Instruction(Opcodes::PUSH, Argument((nargs + 3) * STACK_ENTRY_SIZE), 0, 0));

		if (f->isExternal())
		{
			if (sizeof(ExternalFunctionPtr) == 4)
				instructions.push_back(Instruction(Opcodes::CLE, Argument((int)(int64_t)f->getExternalFunctionPtr()), 0, 0));
			else if (sizeof(ExternalFunctionPtr) == 8)
				instructions.push_back(Instruction(Opcodes::CLE,
					Argument((int)((int64_t)f->getExternalFunctionPtr() >> 32)),
					Argument((int)(int64_t)f->getExternalFunctionPtr()), 0));
			else
				throw NOT_IMPLEMENTED;
		}
		else
			instructions.push_back(Instruction(Opcodes::CL, Argument(f->getAddress()), 0, 0));

		instructions.push_back(Instruction(Opcodes::POP, Argument((nargs + 3) * STACK_ENTRY_SIZE), 0, 0));

		lexer->next();

		return -4;
	}

	DataType Parser::generalizedTypecast(DataType t, DataType s, int tSlot, int sSlot)
	{
		if (t == DataType::F32 && s != DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTF,
				Argument(sSlot),
				Argument(sSlot), 0));
			return DataType::F32;
		}
		else if (t != DataType::F32 && s == DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTF,
				Argument(tSlot),
				Argument(tSlot), 0));
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
				Argument(tSlot),
				Argument(tSlot), 0));
		}
		else if (t != DataType::F32 && s == DataType::F32)
		{
			instructions.push_back(Instruction(Opcodes::CTI,
				Argument(tSlot),
				Argument(tSlot), 0));
		}
		else if (t == DataType::S8 || t == DataType::U8)
		{
			int immValSlot;
			PUSH(immValSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LDI,
				Argument(immValSlot),
				Argument(0xFF), 0));
			instructions.push_back(Instruction(Opcodes::BAND,
				Argument(tSlot),
				Argument(tSlot),
				Argument(immValSlot)));
		}
		else if (t == DataType::S16 || t == DataType::U16)
		{
			int immValSlot;
			PUSH(immValSlot);
			POP();

			instructions.push_back(Instruction(Opcodes::LDI,
				Argument(immValSlot),
				Argument(0xFFFF), 0));
			instructions.push_back(Instruction(Opcodes::BAND,
				Argument(tSlot),
				Argument(tSlot),
				Argument(immValSlot)));
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
		else if (t1->gettype() == Keywords::T_VOID)
			varType = DataType::VOID;

		if (varType != DataType::NO_TYPE)
			lexer->next();

		return varType;
	}

	DataType Parser::globaldecl()
	{
		DataType varType = datatype();
		Token* t2 = lexer->look();

		// function decls without type default to s32
		if (varType == DataType::NO_TYPE && t2->gettype() == TokenTypes::ID && lexer->lookahead(1)->gettype() == '(')
			varType = DataType::S32;

		if (varType != DataType::NO_TYPE)
		{
			if (t2->gettype() != TokenTypes::ID)
				throw exceptions::ID_EXPECTED;

			char* ident = dynamic_cast<Identifier*>(t2)->get();
			Variable* v = getVariable(ident);
			Function* f = getFunction(ident);
			if (v || f)
				throw exceptions::SYMBOL_REDEFINITION;

			lexer->next();
			if (lexer->look()->gettype() == '(')
			{
				int functionStart = instructions.size();
				int nargs = 0;

				// push return value placeholder
				PUSH_PLACEHOLDER();

				// parse arguments
				lexer->next();
				while (!lexer->isEndOfTokenList() && lexer->look()->gettype() != ')')
				{
					if (nargs > 0)
					{
						if (lexer->look()->gettype() != ',')
							throw COMMA_EXPECTED;
						lexer->next();
					}

					DataType argType = datatype();
					if (argType == DataType::NO_TYPE)
						argType = DataType::S32;

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

				functions.push_back(Function(ident, varType, functionStart));
				instructions.push_back(Instruction(Opcodes::PUSH, 0, 0, 0));

				// do function
				stmts();

				// update stack size
				instructions[functionStart].set(Opcodes::PUSH, Argument(maxLocalStackSize), 0, 0);
				int functionEnd = instructions.size();
				instructions.push_back(Instruction(Opcodes::POP, Argument(maxLocalStackSize), 0, 0));
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
				variables.push_back(Variable(varType, ident, globalStackSize, true));
				PUSH_GLOBAL();

				if (lexer->look()->gettype() != ';')
					throw exceptions::SEMICOLON_EXPECTED;
				lexer->next();
			}
		}
		return varType;
	}

	void Parser::loadVarToStack(int stackSlot, Variable* v)
	{
		if (typesizes[v->gettype()] == 1)
			instructions.push_back(Instruction(Opcodes::MB,
				Argument(stackSlot),
				Argument(v->getstackslot()),
				Argument(v->isglobal())));
		else if (typesizes[v->gettype()] == 2)
			instructions.push_back(Instruction(Opcodes::MH,
				Argument(stackSlot),
				Argument(v->getstackslot()),
				Argument(v->isglobal())));
		else if (typesizes[v->gettype()] == 4)
			instructions.push_back(Instruction(Opcodes::MW,
				Argument(stackSlot),
				Argument(v->getstackslot()),
				Argument(v->isglobal())));
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