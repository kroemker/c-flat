#include "Parser.h"

namespace cflat
{

	Parser::Parser(Lexer* lexer) : lexer(lexer)
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
		catch (texceptions i)
		{
			Exception* e = new Exception(i, lexer->getExpressionCount(), lexer->look()->getline());
			lexer->next();
			return e;
		}
		return NULL;
	}

	void Parser::stmts()
	{
		decls();
		Token* t0 = lexer->look();
		if (t0->gettype() == ';')
			lexer->next();
		else if (t0->gettype() == '{')
		{
			lexer->next();
			while (lexer->look()->gettype() != '}')
				stmts();
			lexer->next();
		}
		else if (t0->gettype() == keywords::WHILE)
		{
			lexer->next();
			int position = lexer->getTokenIndex();
			Variable* expr = boolexpr();
			while (expr->getbval() == true)
			{
				stmts();
				lexer->jump(position);
				expr = boolexpr();
			}
			if (lexer->look()->gettype() == '{')
			{
				int begincounter = 1;
				lexer->next();
				while (begincounter > 0)
				{
					if (lexer->look()->gettype() == '{')
						begincounter++;
					else if (lexer->look()->gettype() == '}')
						begincounter--;
					lexer->next();
				}
			}
			else
			{
				while (lexer->look()->gettype() != ';')
					lexer->next();
				lexer->next();
			}
		}
		else if (t0->gettype() == keywords::IF)
		{
			lexer->next();
			Variable* expr = boolexpr();
			if (expr->getbval() == true)
				stmts();
			else
			{
				if (lexer->look()->gettype() == '{')
				{
					int blockcounter = 1;
					lexer->next();
					while (blockcounter > 0)
					{
						if (lexer->look()->gettype() == '{')
							blockcounter++;
						else if (lexer->look()->gettype() == '}')
							blockcounter--;
						lexer->next();
					}
					if (lexer->look()->gettype() == keywords::ELSE)
					{
						lexer->next();
						stmts();
					}
				}
				else
				{
					while (lexer->look()->gettype() != ';')
						lexer->next();
					lexer->next();
					if (lexer->look()->gettype() == keywords::ELSE)
					{
						lexer->next();
						stmts();
					}
				}
			}
		}
		else if (t0->gettype() == ttypes::ID)
		{
			lexer->next();
			Token* t1 = lexer->look();
			if (t1->gettype() == '=')
			{
				std::list<Variable*>::iterator iter;
				Variable* v = NULL;
				for (iter = variables.begin(); iter != variables.end(); iter++)
				{
					if (strcmp(dynamic_cast<Identifier*>(t0)->get(), (*iter)->getname()) == 0)
					{
						v = (*iter);
						break;
					}
				}
				if (v == NULL)
					throw texceptions::UNDEFINED_SYMBOL;
				lexer->next();
				if (v->gettype() == tresult::RES_INTEGER)
					v->setval(mathexpr(tresult::RES_INTEGER, false)->getival());
				else if (v->gettype() == tresult::RES_FLOAT)
					v->setval(mathexpr(tresult::RES_FLOAT, false)->getrval());
				else
					v->setval(boolexpr()->getbval());
				lexer->next();
			}
			else if (t1->gettype() == '(')
			{
				lexer->next();
				Identifier* i0 = dynamic_cast<Identifier*>(t0);
				if (lexer->look()->gettype() == ttypes::STRING)
				{
					if (strcmp(i0->get(), "print") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(lexer->look());
						std::cout << s->get();
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "println") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(lexer->look());
						std::cout << s->get() << '\n';
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;

					}
					else
						throw texceptions::UNDEFINED_SYMBOL;
				}
				else
				{
					if (strcmp(i0->get(), "intout") == 0)
					{
						Variable* k = mathexpr(tresult::RES_INTEGER, false);
						std::cout << k->getival();
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "realout") == 0)
					{
						Variable* k = mathexpr(tresult::RES_FLOAT, false);
						std::cout << k->getrval();
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "boolout") == 0)
					{
						Variable* k = boolexpr();
						if (k->getbval() == true)
							std::cout << "true";
						else
							std::cout << "false";
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
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
							throw texceptions::UNDEFINED_SYMBOL;
						if (v->gettype() == tresult::RES_BOOLEAN)
						{
							bool b = false;
							std::cin >> b;
							v->setval(b);
						}
						else if (v->gettype() == tresult::RES_INTEGER)
						{
							int i = 0;
							std::cin >> i;
							v->setval(i);
						}
						else if (v->gettype() == tresult::RES_FLOAT)
						{
							REALNUM r = 0;
							std::cin >> r;
							v->setval(r);
						}
						lexer->next();
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "wait") == 0)
					{
						system("PAUSE");
						if (lexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
				}
				lexer->next();
				if (lexer->look()->gettype() != ';')
					throw texceptions::SEMICOLON_EXPECTED;
				lexer->next();
			}
			else
				throw texceptions::ASSIGNORFUNCTIONCALL_EXPECTED;
		}
		else
			throw texceptions::INVALID_CHARACTER;
	}

	Variable* Parser::boolexpr()
	{
		Variable* r = joinexpr();
		Variable* k = r;
		if (lexer->look()->gettype() == keywords::OR)
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			Variable* h = joinexpr();
			if ((r->gettype() == tresult::RES_BOOLEAN) && (k->gettype() == tresult::RES_BOOLEAN))
				k->setval(r->getbval() || h->getbval());
			else
				throw texceptions::TYPEINCONGRUENCE;
		}
		return k;
	}

	Variable* Parser::joinexpr()
	{
		Variable* r = equalityexpr();
		Variable* k = r;
		if (lexer->look()->gettype() == keywords::AND)
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			Variable* h = equalityexpr();
			if ((r->gettype() == tresult::RES_BOOLEAN) && (k->gettype() == tresult::RES_BOOLEAN))
				k->setval(r->getbval() && h->getbval());
			else
				throw texceptions::TYPEINCONGRUENCE;
		}
		return k;
	}

	Variable* Parser::equalityexpr()
	{
		Variable* r = relexpr();
		Variable* k = r;
		if (lexer->look()->gettype() == ttypes::EQUALS)
		{
			lexer->next();
			Variable* p = relexpr();
			if (!(p->gettype() == r->gettype()))
				throw texceptions::TYPEINCONGRUENCE;

			k = new Variable(tresult::RES_BOOLEAN, NULL);
			if (p->gettype() == tresult::RES_BOOLEAN)
				k->setval(r->getbval() == p->getbval());
			else
				k->setval(r->getrval() == p->getrval());
		}
		else if (lexer->look()->gettype() == ttypes::UNEQUALS)
		{
			lexer->next();
			Variable* p = relexpr();
			if (!(p->gettype() == r->gettype()))
				throw texceptions::TYPEINCONGRUENCE;

			k = new Variable(tresult::RES_BOOLEAN, NULL);
			if (p->gettype() == tresult::RES_BOOLEAN)
				k->setval(r->getbval() != p->getbval());
			else
				k->setval(r->getrval() != p->getrval());
		}
		return k;
	}

	Variable* Parser::relexpr()
	{
		Variable* r = mathexpr(tresult::RES_FLOAT, true);
		Variable* k = r;
		if (lexer->look()->gettype() == '<')
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() < mathexpr(tresult::RES_FLOAT, true)->getrval());
		}
		else if (lexer->look()->gettype() == ttypes::LESS_EQUALS)
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() <= mathexpr(tresult::RES_FLOAT, true)->getrval());
		}
		else if (lexer->look()->gettype() == '>')
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() > mathexpr(tresult::RES_FLOAT, true)->getrval());
		}
		else if (lexer->look()->gettype() == ttypes::GREATER_EQUALS)
		{
			lexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() >= mathexpr(tresult::RES_FLOAT, true)->getrval());
		}
		return k;
	}

	Variable* Parser::mathexpr(int numerictype, bool isboolexpr)
	{
		Variable *v = mathterm(numerictype, isboolexpr);
		while ((lexer->look()->gettype() == '+') || (lexer->look()->gettype() == '-'))
		{
			if (lexer->look()->gettype() == '+')
			{
				lexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() + mathterm(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_FLOAT)
					v->setval(v->getrval() + mathterm(numerictype, isboolexpr)->getrval());
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
			else if (lexer->look()->gettype() == '-')
			{
				lexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() - mathterm(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_FLOAT)
					v->setval(v->getrval() - mathterm(numerictype, isboolexpr)->getrval());
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
		}
		return v;
	}

	Variable* Parser::mathterm(int numerictype, bool isboolexpr)
	{
		Variable *v = mathunary(numerictype, isboolexpr);
		while ((lexer->look()->gettype() == '*') || (lexer->look()->gettype() == '/'))
		{
			if (lexer->look()->gettype() == '*')
			{
				lexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() * mathunary(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_FLOAT)
					v->setval(v->getrval() * mathunary(numerictype, isboolexpr)->getrval());
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
			else if (lexer->look()->gettype() == '/')
			{
				lexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(static_cast<int>(v->getival() / mathunary(numerictype, isboolexpr)->getival()));
				else if (numerictype == tresult::RES_FLOAT)
					v->setval(static_cast<REALNUM>(v->getrval() / mathunary(numerictype, isboolexpr)->getrval()));
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
		}
		return v;
	}

	Variable* Parser::mathunary(int numerictype, bool isboolexpr)
	{
		if (lexer->look()->gettype() == '-')
		{
			lexer->next();
			Variable* k = new Variable(numerictype, NULL);
			if (numerictype == tresult::RES_INTEGER)
				k->setval(-mathunary(numerictype, isboolexpr)->getival());
			else if (numerictype == tresult::RES_FLOAT)
				k->setval(-mathunary(numerictype, isboolexpr)->getrval());
			else
				throw texceptions::BOOLEANILLEGALLYFOUND;
			return k;
		}
		else if (lexer->look()->gettype() == keywords::NOT)
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANOPERATORTYPEFAULT;
			lexer->next();
			Variable* n = new Variable(tresult::RES_BOOLEAN, NULL);
			n->setval(!(mathunary(numerictype, true)->getbval()));
			return n;
		}
		return factor(numerictype, isboolexpr);
	}

	Variable* Parser::factor(int numerictype, bool isboolexpr)
	{
		switch (lexer->look()->gettype())
		{
		case '(':
		{
			lexer->next();
			Variable* v;
			if (isboolexpr)
				v = boolexpr();
			else
				v = mathexpr(numerictype, false);

			if (lexer->look()->gettype() == ')')
				lexer->next();
			else
				throw texceptions::CLOSEBRACKET_EXPECTED;
			return v;
			break;
		}
		case ttypes::INTEGER:
		{
			Variable* c;
			if (numerictype == tresult::RES_FLOAT)
			{
				c = new Variable(tresult::RES_FLOAT, NULL);
				c->setval(static_cast<REALNUM>(dynamic_cast<Integer*>(lexer->look())->getvalue()));
			}
			else
			{
				c = new Variable(tresult::RES_INTEGER, NULL);
				c->setval(dynamic_cast<Integer*>(lexer->look())->getvalue());
			}
			lexer->next();
			return c;
			break;
		}
		case ttypes::REALLIT:
		{
			Variable* x;
			if (numerictype == tresult::RES_INTEGER)
			{
				x = new Variable(tresult::RES_INTEGER, NULL);
				x->setval(static_cast<int>(dynamic_cast<Real*>(lexer->look())->getvalue()));
			}
			else
			{
				x = new Variable(tresult::RES_FLOAT, NULL);
				x->setval(dynamic_cast<Real*>(lexer->look())->getvalue());
			}
			lexer->next();
			return x;
			break;
		}
		case keywords::TRUE:
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			Variable* l = new Variable(tresult::RES_BOOLEAN, NULL);
			l->setval(true);
			lexer->next();
			return l;
			break;
		}
		case keywords::FALSE:
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			Variable* l = new Variable(tresult::RES_BOOLEAN, NULL);
			l->setval(false);
			lexer->next();
			return l;
			break;
		}
		case ttypes::ID:
		{
			std::list<Variable*>::iterator iter;
			for (iter = variables.begin(); iter != variables.end(); iter++)
			{
				if (strcmp(dynamic_cast<Identifier*>(lexer->look())->get(), (*iter)->getname()) == 0)
				{
					lexer->next();
					return (*iter);
				}
			}
			break;
		}
		}
	}

	void Parser::decls()
	{
		while (decl() != 0)
		{
			if (lexer->look()->gettype() != ';')
				throw texceptions::SEMICOLON_EXPECTED;
			lexer->next();
		}
	}

	int Parser::decl()
	{
		int varType = 0;
		Token* t1 = lexer->look();
		if (t1->gettype() == keywords::INT)
			varType = tresult::RES_INTEGER;
		else if (t1->gettype() == keywords::FLOAT)
			varType = tresult::RES_FLOAT;
		else if (t1->gettype() == keywords::BOOL)
			varType = tresult::RES_BOOLEAN;

		if (varType != 0)
		{
			lexer->next();
			Token* t2 = lexer->look();
			if (t2->gettype() == ttypes::ID)
			{
				char* ident = dynamic_cast<Identifier*>(t2)->get();
				for (auto it = variables.begin(); it != variables.end(); ++it)
				{
					if (strcmp((*it)->getname(), ident) == 0)
						throw texceptions::SYMBOL_REDEFINITION;
				}
				variables.push_back(new Variable(varType, ident));
			}
			else
				throw texceptions::ID_EXPECTED;
			lexer->next();
		}
		return varType;
	}

}