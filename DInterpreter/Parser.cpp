#include "Parser.h"

namespace cscript
{

	Parser::Parser(Lexer* pLex) : pLexer(pLex)
	{
		if (!pLex->isPrelexed())
			pLex->prelex();
	}

	Parser::~Parser(void)
	{
	}

	Exception* Parser::parsenext()
	{
		try
		{
			Token* t0 = pLexer->look();
			if (t0->gettype() == keywords::VAR)
			{
				pLexer->next();
				decls();
			}
			stmts();
		}
		catch (texceptions i)
		{
			return new Exception(i, pLexer->getExpressionCount());
		}
		return NULL;
	}

	void Parser::stmts()
	{
		Token* t0 = pLexer->look();
		if (t0->gettype() == '{')
		{
			pLexer->next();
			while (pLexer->look()->gettype() != '}')
				stmts();
			pLexer->next();
			if (pLexer->look()->gettype() != ';')
				throw texceptions::SEMICOLON_EXPECTED;
			pLexer->next();
		}
		else if (t0->gettype() == keywords::WHILE)
		{
			pLexer->next();
			int position = pLexer->getTokenIndex();
			Variable* expr = boolexpr();
			if (pLexer->look()->gettype() == keywords::DO)
			{
				pLexer->next();
				while (expr->getbval() == true)
				{
					stmts();
					pLexer->jump(position);
					expr = boolexpr();
					pLexer->next();
				}
				if (pLexer->look()->gettype() == '{')
				{
					int begincounter = 1;
					pLexer->next();
					while (begincounter > 0)
					{
						if (pLexer->look()->gettype() == '{')
							begincounter++;
						else if (pLexer->look()->gettype() == '}')
						{
							pLexer->next();
							if (pLexer->look()->gettype() != ';')
								throw texceptions::SEMICOLON_EXPECTED;
							begincounter--;
						}
						pLexer->next();
					}
				}
				else
				{
					while (pLexer->look()->gettype() != ';')
						pLexer->next();
					pLexer->next();
				}
			}
			else
				throw texceptions::DOEXPECTED;
		}
		else if (t0->gettype() == keywords::IF)
		{
			pLexer->next();
			Variable* expr = boolexpr();
			if (pLexer->look()->gettype() == keywords::THEN)
			{
				pLexer->next();
				if (expr->getbval() == true)
					stmts();
				else
				{
					if (pLexer->look()->gettype() == '{')
					{
						int begincounter = 1;
						pLexer->next();
						while (begincounter > 0)
						{
							if (pLexer->look()->gettype() == '{')
								begincounter++;
							else if (pLexer->look()->gettype() == '}')
							{
								pLexer->next();
								if (pLexer->look()->gettype() != ';')
									throw texceptions::SEMICOLON_EXPECTED;
								begincounter--;
							}
							pLexer->next();
						}
						if (pLexer->look()->gettype() == keywords::ELSE)
						{
							pLexer->next();
							stmts();
						}
					}
					else
					{
						while (pLexer->look()->gettype() != ';')
							pLexer->next();
						pLexer->next();
						if (pLexer->look()->gettype() == keywords::ELSE)
						{
							pLexer->next();
							stmts();
						}
					}
				}
			}
			else
				throw texceptions::THENEXPECTED;
		}
		else if (t0->gettype() == ttypes::ID)
		{
			pLexer->next();
			Token* t1 = pLexer->look();
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
				pLexer->next();
				if (v->gettype() == tresult::RES_INTEGER)
					v->setval(mathexpr(tresult::RES_INTEGER, false)->getival());
				else if (v->gettype() == tresult::RES_REAL)
					v->setval(mathexpr(tresult::RES_REAL, false)->getrval());
				else
					v->setval(boolexpr()->getbval());
				pLexer->next();
			}
			else if (t1->gettype() == '(')
			{
				pLexer->next();
				Identifier* i0 = dynamic_cast<Identifier*>(t0);
				if (pLexer->look()->gettype() == ttypes::STRING)
				{
					if (strcmp(i0->get(), "print") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(pLexer->look());
						std::cout << s->get();
						pLexer->next();
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "println") == 0)
					{
						Stringlit* s = dynamic_cast<Stringlit*>(pLexer->look());
						std::cout << s->get() << '\n';
						pLexer->next();
						if (pLexer->look()->gettype() != ')')
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
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "realout") == 0)
					{
						Variable* k = mathexpr(tresult::RES_REAL, false);
						std::cout << k->getrval();
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "boolout") == 0)
					{
						Variable* k = boolexpr();
						if (k->getbval() == true)
							std::cout << "true";
						else
							std::cout << "false";
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "in") == 0)
					{
						std::list<Variable*>::iterator iter;
						Variable* v = NULL;
						for (iter = variables.begin(); iter != variables.end(); iter++)
						{
							if (strcmp(dynamic_cast<Identifier*>(pLexer->look())->get(), (*iter)->getname()) == 0)
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
						else if (v->gettype() == tresult::RES_REAL)
						{
							REALNUM r = 0;
							std::cin >> r;
							v->setval(r);
						}
						pLexer->next();
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
					else if (strcmp(i0->get(), "wait") == 0)
					{
						system("PAUSE");
						if (pLexer->look()->gettype() != ')')
							throw texceptions::CLOSEBRACKET_EXPECTED;
					}
				}
				pLexer->next();
				if (pLexer->look()->gettype() != ';')
					throw texceptions::SEMICOLON_EXPECTED;
				pLexer->next();
			}
			else
				throw texceptions::ASSIGNORFUNCTIONCALL_EXPECTED;
		}
	}

	Variable* Parser::boolexpr()
	{
		Variable* r = joinexpr();
		Variable* k = r;
		if (pLexer->look()->gettype() == keywords::OR)
		{
			pLexer->next();
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
		if (pLexer->look()->gettype() == keywords::AND)
		{
			pLexer->next();
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
		if (pLexer->look()->gettype() == ttypes::EQUALS)
		{
			pLexer->next();
			Variable* p = relexpr();
			if (!(p->gettype() == r->gettype()))
				throw texceptions::TYPEINCONGRUENCE;

			k = new Variable(tresult::RES_BOOLEAN, NULL);
			if (p->gettype() == tresult::RES_BOOLEAN)
				k->setval(r->getbval() == p->getbval());
			else
				k->setval(r->getrval() == p->getrval());
		}
		else if (pLexer->look()->gettype() == ttypes::UNEQUALS)
		{
			pLexer->next();
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
		Variable* r = mathexpr(tresult::RES_REAL, true);
		Variable* k = r;
		if (pLexer->look()->gettype() == '<')
		{
			pLexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() < mathexpr(tresult::RES_REAL, true)->getrval());
		}
		else if (pLexer->look()->gettype() == ttypes::LESS_EQUALS)
		{
			pLexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() <= mathexpr(tresult::RES_REAL, true)->getrval());
		}
		else if (pLexer->look()->gettype() == '>')
		{
			pLexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() > mathexpr(tresult::RES_REAL, true)->getrval());
		}
		else if (pLexer->look()->gettype() == ttypes::GREATER_EQUALS)
		{
			pLexer->next();
			k = new Variable(tresult::RES_BOOLEAN, NULL);
			k->setval(r->getrval() >= mathexpr(tresult::RES_REAL, true)->getrval());
		}
		return k;
	}

	Variable* Parser::mathexpr(int numerictype, bool isboolexpr)
	{
		Variable *v = mathterm(numerictype, isboolexpr);
		while ((pLexer->look()->gettype() == '+') || (pLexer->look()->gettype() == '-'))
		{
			if (pLexer->look()->gettype() == '+')
			{
				pLexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() + mathterm(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_REAL)
					v->setval(v->getrval() + mathterm(numerictype, isboolexpr)->getrval());
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
			else if (pLexer->look()->gettype() == '-')
			{
				pLexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() - mathterm(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_REAL)
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
		while ((pLexer->look()->gettype() == '*') || (pLexer->look()->gettype() == '/'))
		{
			if (pLexer->look()->gettype() == '*')
			{
				pLexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(v->getival() * mathunary(numerictype, isboolexpr)->getival());
				else if (numerictype == tresult::RES_REAL)
					v->setval(v->getrval() * mathunary(numerictype, isboolexpr)->getrval());
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
			else if (pLexer->look()->gettype() == '/')
			{
				pLexer->next();
				if (numerictype == tresult::RES_INTEGER)
					v->setval(static_cast<int>(v->getival() / mathunary(numerictype, isboolexpr)->getival()));
				else if (numerictype == tresult::RES_REAL)
					v->setval(static_cast<REALNUM>(v->getrval() / mathunary(numerictype, isboolexpr)->getrval()));
				else
					throw texceptions::BOOLEANILLEGALLYFOUND;
			}
		}
		return v;
	}

	Variable* Parser::mathunary(int numerictype, bool isboolexpr)
	{
		if (pLexer->look()->gettype() == '-')
		{
			pLexer->next();
			Variable* k = new Variable(numerictype, NULL);
			if (numerictype == tresult::RES_INTEGER)
				k->setval(-mathunary(numerictype, isboolexpr)->getival());
			else if (numerictype == tresult::RES_REAL)
				k->setval(-mathunary(numerictype, isboolexpr)->getrval());
			else
				throw texceptions::BOOLEANILLEGALLYFOUND;
			return k;
		}
		else if (pLexer->look()->gettype() == keywords::NOT)
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANOPERATORTYPEFAULT;
			pLexer->next();
			Variable* n = new Variable(tresult::RES_BOOLEAN, NULL);
			n->setval(!(mathunary(numerictype, true)->getbval()));
			return n;
		}
		return factor(numerictype, isboolexpr);
	}

	Variable* Parser::factor(int numerictype, bool isboolexpr)
	{
		switch (pLexer->look()->gettype())
		{
		case '(':
		{
			pLexer->next();
			Variable* v;
			if (isboolexpr)
				v = boolexpr();
			else
				v = mathexpr(numerictype, false);

			if (pLexer->look()->gettype() == ')')
				pLexer->next();
			else
				throw texceptions::CLOSEBRACKET_EXPECTED;
			return v;
			break;
		}
		case ttypes::INT:
		{
			Variable* c;
			if (numerictype == tresult::RES_REAL)
			{
				c = new Variable(tresult::RES_REAL, NULL);
				c->setval(static_cast<REALNUM>(dynamic_cast<Integer*>(pLexer->look())->getvalue()));
			}
			else
			{
				c = new Variable(tresult::RES_INTEGER, NULL);
				c->setval(dynamic_cast<Integer*>(pLexer->look())->getvalue());
			}
			pLexer->next();
			return c;
			break;
		}
		case ttypes::REALLIT:
		{
			Variable* x;
			if (numerictype == tresult::RES_INTEGER)
			{
				x = new Variable(tresult::RES_INTEGER, NULL);
				x->setval(static_cast<int>(dynamic_cast<Real*>(pLexer->look())->getvalue()));
			}
			else
			{
				x = new Variable(tresult::RES_REAL, NULL);
				x->setval(dynamic_cast<Real*>(pLexer->look())->getvalue());
			}
			pLexer->next();
			return x;
			break;
		}
		case keywords::TRUE:
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			Variable* l = new Variable(tresult::RES_BOOLEAN, NULL);
			l->setval(true);
			pLexer->next();
			return l;
			break;
		}
		case keywords::FALSE:
		{
			if (!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			Variable* l = new Variable(tresult::RES_BOOLEAN, NULL);
			l->setval(false);
			pLexer->next();
			return l;
			break;
		}
		case ttypes::ID:
		{
			std::list<Variable*>::iterator iter;
			for (iter = variables.begin(); iter != variables.end(); iter++)
			{
				if (strcmp(dynamic_cast<Identifier*>(pLexer->look())->get(), (*iter)->getname()) == 0)
				{
					pLexer->next();
					return (*iter);
				}
			}
			break;
		}
		}
	}

	void Parser::decls()
	{
		while (pLexer->look()->gettype() != '{')
		{
			decl();
			pLexer->next();
		}
	}

	void Parser::decl()
	{
		Token* t1 = pLexer->look();
		if (t1->gettype() == ttypes::ID)
		{
			pLexer->next();
			Token* t2 = pLexer->look();
			if (t2->gettype() == ':')
			{
				pLexer->next();
				Token* t3 = pLexer->look();
				if (t3->gettype() == keywords::INTEGER)
				{
					variables.push_back(new Variable(tresult::RES_INTEGER, dynamic_cast<Identifier*>(t1)->get()));
				}
				else if (t3->gettype() == keywords::REAL)
				{
					variables.push_back(new Variable(tresult::RES_REAL, dynamic_cast<Identifier*>(t1)->get()));
				}
				else if (t3->gettype() == keywords::BOOLEAN)
				{
					variables.push_back(new Variable(tresult::RES_BOOLEAN, dynamic_cast<Identifier*>(t1)->get()));
				}
			}
			pLexer->next();
		}
	}

}