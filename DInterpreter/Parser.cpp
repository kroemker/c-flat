#include "Parser.h"

#define PUSH(name)	name = stackSize; stackSize += 4
#define POP()		stackSize -= 4

namespace cflat
{

	Parser::Parser(Lexer* lexer) : lexer(lexer), stackSize(0)
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
			stmts();
		}
		catch (exceptions i)
		{
			Exception* e = new Exception(i, lexer->getExpressionCount(), lexer->look()->getline());
			lexer->next();
			return e;
		}
		return NULL;
	}

	void Parser::stmts()
	{
		decls(false);
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
			Token* t1 = lexer->look();
			if (t1->gettype() == '=')
			{
				Variable* v = getvariable(dynamic_cast<Identifier*>(t0)->get());
				if (v == NULL)
					throw exceptions::UNDEFINED_SYMBOL;
				lexer->next();
				DataType t = boolexpr(v->getstackslot());
				forcedTypecast(v->gettype(), t, v->getstackslot());
			}
			/*else if (t1->gettype() == '(')
			{
				lexer->next();
				Identifier* i0 = dynamic_cast<Identifier*>(t0);
				if (lexer->look()->gettype() == TokenTypes::STRING)
				{
					if (strcmp(i0->get(), "print") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(lexer->look());
						std::cout << s->get();
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "println") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(lexer->look());
						std::cout << s->get() << '\n';
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;

					}
					else
						throw exceptions::UNDEFINED_SYMBOL;
				}
				else
				{
					if (strcmp(i0->get(), "intout") == 0)
					{
						Variable* k = mathexpr(DataType::RES_INTEGER, false);
						std::cout << k->getival();
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "realout") == 0)
					{
						Variable* k = mathexpr(DataType::RES_FLOAT, false);
						std::cout << k->getrval();
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "boolout") == 0)
					{
						Variable* k = boolexpr();
						if (k->getbval() == true)
							std::cout << "true";
						else
							std::cout << "false";
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "in") == 0)
					{
						std::list<Variable*>::iterator iter;
						Variable* v = NULL;
						for (iter = variables.begin(); iter != variables.end(); iter++)
						{
							if (strcmp(dynamic_cast<Identifier*>(lexer->look())->get(), (*iter)->getname()) == 0)
							{
								v = (*iter);
								break;
							}
						}
						if (v == NULL)
							throw exceptions::UNDEFINED_SYMBOL;
						if (v->gettype() == DataType::RES_BOOLEAN)
						{
							bool b = false;
							std::cin >> b;
							v->setval(b);
						}
						else if (v->gettype() == DataType::RES_INTEGER)
						{
							int i = 0;
							std::cin >> i;
							v->setval(i);
						}
						else if (v->gettype() == DataType::RES_FLOAT)
						{
							REALNUM r = 0;
							std::cin >> r;
							v->setval(r);
						}
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "wait") == 0)
					{
						system("PAUSE");
						if (lexer->look()->gettype() != ')')
							throw exceptions::CLOSEBRACKET_EXPECTED;
					}
				}
				lexer->next();
			}*/
			else
				throw exceptions::ASSIGNORFUNCTIONCALL_EXPECTED;

			if (lexer->look()->gettype() != ';')
				throw exceptions::SEMICOLON_EXPECTED;
			lexer->next();
		}
		else
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
					return DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::ADD,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					return DataType::S32;
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
					return DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::SUB,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					return DataType::S32;
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
					return DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::MULT,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					return DataType::S32;
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
					return DataType::F32;
				}
				else
				{
					instructions.push_back(Instruction(Opcodes::DIV,
						Instruction::Arg(stackSlot),
						Instruction::Arg(stackSlot),
						Instruction::Arg(newSlot)));
					return DataType::S32;
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
				Instruction::Arg(stackSlot + 4), 0, 0));

			if (t == DataType::F32)
				instructions.push_back(Instruction(Opcodes::SUBF,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot + 4),
					Instruction::Arg(stackSlot)));
			else
				instructions.push_back(Instruction(Opcodes::SUB,
					Instruction::Arg(stackSlot),
					Instruction::Arg(stackSlot + 4),
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
			Variable* v = getvariable(dynamic_cast<Identifier*>(lexer->look())->get());
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
				lexer->next();
			}
			else
				throw exceptions::UNDEFINED_SYMBOL;
			return v->gettype();
		}
		}
		throw exceptions::UNDEFINED_SYMBOL;
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

	void Parser::decls(bool allowFuncs)
	{
		int declType;
		while ((declType = decl(allowFuncs)) != 0)
		{
			if (declType != 255 && lexer->look()->gettype() != ';')
				throw exceptions::SEMICOLON_EXPECTED;
			lexer->next();
		}
	}

	int Parser::decl(bool allowFuncs)
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

		if (varType != 0)
		{
			lexer->next();
			Token* t2 = lexer->look();
			if (t2->gettype() == TokenTypes::ID)
			{
				char* ident = dynamic_cast<Identifier*>(t2)->get();
				Variable* v = getvariable(ident);
				if (v)
					throw exceptions::SYMBOL_REDEFINITION;

				variables.push_back(Variable(varType, ident, stackSize));
				stackSize += 4; // every type is 32 bits wide
			}
			else
				throw exceptions::ID_EXPECTED;
			lexer->next();
		}
		return varType;
	}

	Variable* Parser::getvariable(char* name)
	{
		for (int i = 0; i < variables.size(); i++)
		{
			if (strcmp(name, variables[i].getname()) == 0)
				return &variables[i];
		}
		return NULL;
	}

}