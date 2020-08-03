#include "Parser.h"

namespace inter
{

Parser::Parser(Lexer* pLex) : pLexer(pLex)
{
	if(!pLex->isprelexed()) // Tokenstream erzeugen
		pLex->prelex();
}

Parser::~Parser(void)
{
}

exception* Parser::parsenext() // <stmts> // <var> // 
{
	try
	{
		token* t0 = pLexer->look();
		//Variablen bearbeiten falls Deklarationen vorhanden
		if(t0->gettype() == keywords::VAR)
		{
			pLexer->next();
			decls();
		}
		stmts();
	}
	catch(texceptions i)
	{	
		return new exception(i,pLexer->getexprnum());
	}
	return NULL;
}
void Parser::stmts()
{
	token* t0 = pLexer->look();
	// auf Block testen
	if(t0->gettype() == keywords::BEGIN)
	{
		pLexer->next();
		// bis END gefunden wird Ausdrücke auswerten (stmts())
		while(pLexer->look()->gettype() != keywords::END)
			stmts();
		pLexer->next(); // ";"
		if(pLexer->look()->gettype() != ';')
			throw texceptions::SEMICOLON_EXPECTED;
		pLexer->next();
	}
	else if(t0->gettype() == keywords::WHILE)
	{
		pLexer->next();
		// Rücksprungposition merken
		int position = pLexer->getstreampos();
		// booleschen Ausdruck auswerten
		variable* expr = boolexpr();
		if(pLexer->look()->gettype() == keywords::DO)
		{
			pLexer->next();
			// solange expr wahr ist stmts() aufrufen 
			// und dann zurückspringen und erneut testen
			while(expr->getbval() == true)
			{
				stmts();
				pLexer->jumpto(position);
				expr = boolexpr();
				pLexer->next();
			}
			if(pLexer->look()->gettype() == keywords::BEGIN) // mehrzeilige WHILE-Anweisung
			{
				int begincounter = 1;
				pLexer->next();
				// Begins und Ends abzählen
				while(begincounter > 0)
				{
					if(pLexer->look()->gettype() == keywords::BEGIN)
						begincounter++;
					else if(pLexer->look()->gettype() == keywords::END)
					{
						pLexer->next();
						if(pLexer->look()->gettype() != ';')
							throw texceptions::SEMICOLON_EXPECTED;
						begincounter--;
					}
					pLexer->next();
				}
			}
			else // einzeilige WHILE-Anweisung überspringen
			{
				// bis zum nächsten Semikolon überspringen
				while(pLexer->look()->gettype() != ';')
					pLexer->next();
				pLexer->next();
			}
		}
		else
			throw texceptions::DOEXPECTED;
	}
	else if(t0->gettype() == keywords::IF) // if- Anweisung überspringen
	{
		pLexer->next();
		variable* expr = boolexpr();
		if(pLexer->look()->gettype() == keywords::THEN)
		{
			pLexer->next();
			if(expr->getbval() == true)
				stmts();
			else // IF-Bedingung ist falsch => auf else Teil überprüfen
			{
				if(pLexer->look()->gettype() == keywords::BEGIN) // mehrzeilige if anweisung überspringen
				{
					int begincounter = 1;
					pLexer->next();
					// Begins und Ends abzählen
					while(begincounter > 0)
					{
						if(pLexer->look()->gettype() == keywords::BEGIN)
							begincounter++;
						else if(pLexer->look()->gettype() == keywords::END)
						{
							pLexer->next();
							if(pLexer->look()->gettype() != ';')
								throw texceptions::SEMICOLON_EXPECTED;
							begincounter--;
						}
						pLexer->next();
					}
					// auf else-Teil prüfen
					if(pLexer->look()->gettype() == keywords::ELSE)
					{
						pLexer->next();
						stmts();
					}
				}
				else // einzeilige if anweisung überspringen
				{
					// bis zum nächsten Semikolon überspringen
					while(pLexer->look()->gettype() != ';')
						pLexer->next();
					pLexer->next();
					// auf else-Teil prüfen
					if(pLexer->look()->gettype() == keywords::ELSE)
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
	else if(t0->gettype() == ttypes::ID) // <ID> 
	{   
		pLexer->next();
		token* t1 = pLexer->look();
		if(t1->gettype() == ttypes::ASSIGN) // :=
		{
			//Variable bestimmen
			std::list<variable*>::iterator iter;
			variable* v = NULL;
			// Iterator durch Variablenliste
			for(iter = variables.begin();iter != variables.end(); iter++)
			{
				if(strcmp(dynamic_cast<identifier*>(t0)->get(),(*iter)->getname())==0)
				{
					v = (*iter);
					break;
				}
			}
			if(v == NULL) // nicht gefunden
				throw texceptions::UNDEFINED_SYMBOL;
			pLexer->next(); 
			if(v->gettype() == tresult::RES_INTEGER)
				v->setval(mathexpr(tresult::RES_INTEGER,false)->getival()); // Integer Berechnungen können nur durch mathexpr erfolgen
			else if(v->gettype() == tresult::RES_REAL)
				v->setval(mathexpr(tresult::RES_REAL,false)->getrval());   // Real Berechnungen können nur durch mathexpr erfolgen
			else
				v->setval(boolexpr()->getbval());
			pLexer->next();
		}
		else if(t1->gettype() == '(') // "(" => Funktionsaufruf
		{
			pLexer->next();
			identifier* i0 = dynamic_cast<identifier*>(t0);
			//Stringliteral -Sonderfälle bestimmen
			if(pLexer->look()->gettype() == ttypes::STRING)
			{
				if(strcmp(i0->get(),"print")==0)
				{
					stringlit* s = dynamic_cast<stringlit*>(pLexer->look()); // umwandeln in Stringlit Zeiger
					std::cout << s->get(); // durch Konsolenstream ausgeben
					pLexer->next(); // auf ')' überprüfen
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
				else if(strcmp(i0->get(),"println")==0)
				{
					stringlit* s = dynamic_cast<stringlit*>(pLexer->look()); // umwandeln in Stringlit Zeiger
					std::cout << s->get() << '\n'; // durch Konsolenstream ausgeben
					pLexer->next(); // auf ')' überprüfen
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
					
				}
				else
					throw texceptions::UNDEFINED_SYMBOL;
			}
			else
			{
				if(strcmp(i0->get(),"intout")==0)
				{
					variable* k = mathexpr(tresult::RES_INTEGER,false); // mathematischen Ausdruck bearbeiten
					std::cout << k->getival(); // durch Konsolenstream ausgeben
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
				else if(strcmp(i0->get(),"realout")==0)
				{
					variable* k = mathexpr(tresult::RES_REAL,false); // mathematischen Ausdruck bearbeiten
					std::cout << k->getrval(); // durch Konsolenstream ausgeben
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
				else if(strcmp(i0->get(),"boolout")==0)
				{
					variable* k = boolexpr(); // booleschen Ausdruck bearbeiten
					// durch Konsolenstream ausgeben
					if(k->getbval() == true)
						std::cout << "true"; 
					else
						std::cout << "false";
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
				else if(strcmp(i0->get(),"in")==0)
				{
					//Variable bestimmen
					std::list<variable*>::iterator iter;
					variable* v = NULL;
					for(iter = variables.begin();iter != variables.end(); iter++)
					{
						if(strcmp(dynamic_cast<identifier*>(pLexer->look())->get(),(*iter)->getname())==0)
						{
							v = (*iter);
							break;
						}
					}
					if(v == NULL)
						throw texceptions::UNDEFINED_SYMBOL;
					//Typüberprüfung
					if(v->gettype() == tresult::RES_BOOLEAN)
					{
						bool b = false;
						std::cin >> b; // Standardeingabestream abfragen
						v->setval(b);
					}
					else if(v->gettype() == tresult::RES_INTEGER)
					{
						int i = 0;
						std::cin >> i; // Standardeingabestream abfragen
						v->setval(i);
					}
					else if(v->gettype() == tresult::RES_REAL)
					{
						REALNUM r = 0;
						std::cin >> r; // Standardeingabestream abfragen
						v->setval(r);
					}
					pLexer->next();
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
				else if(strcmp(i0->get(),"wait")==0)
				{
					system("PAUSE");
					if(pLexer->look()->gettype() != ')')
						throw texceptions::CLOSEBRACKET_EXPECTED;
				}
			}
			pLexer->next(); // <;>
			if(pLexer->look()->gettype() != ';')
				throw texceptions::SEMICOLON_EXPECTED;
			pLexer->next();
		}
		else
			throw texceptions::ASSIGNORFUNCTIONCALL_EXPECTED;
	}
}
variable* Parser::boolexpr() // boolescher Ausdruck
{
	variable* r = joinexpr();
	variable* k = r;
	if(pLexer->look()->gettype() == keywords::OR) //<boolexpr> "or" <joinexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);
		variable* h = joinexpr();
		if((r->gettype() == tresult::RES_BOOLEAN) && (k->gettype() == tresult::RES_BOOLEAN))
			k->setval(r->getbval() || h->getbval());
		else
			throw texceptions::TYPEINCONGRUENCE;
	}
	return k;
}

variable* Parser::joinexpr()
{
	variable* r = equalityexpr();
	variable* k = r;
	if(pLexer->look()->gettype() == keywords::AND) //<boolexpr> "and" <equalityexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);
		variable* h = equalityexpr();
		if((r->gettype() == tresult::RES_BOOLEAN) && (k->gettype() == tresult::RES_BOOLEAN))
			k->setval(r->getbval() && h->getbval());
		else
			throw texceptions::TYPEINCONGRUENCE;
	}
	return k;
}

variable* Parser::equalityexpr()
{
	variable* r = relexpr();
	variable* k = r;
	if(pLexer->look()->gettype() == '=') //<equalityexpr> "=" <relexpr> 
	{
		pLexer->next();
		variable* p = relexpr();
		if(!(p->gettype()==r->gettype()))		//Typenvalidierung
			throw texceptions::TYPEINCONGRUENCE;

		k = new variable(tresult::RES_BOOLEAN,NULL);
		if(p->gettype() == tresult::RES_BOOLEAN)
			k->setval(r->getbval() == p->getbval());
		else // real
			k->setval(r->getrval() == p->getrval());
	}
	else if(pLexer->look()->gettype() == ttypes::UNEQUALS) //<equalityexpr> "<>" <relexpr>
	{
		pLexer->next();
		variable* p = relexpr();
		if(!(p->gettype()==r->gettype()))		//Typenvalidierung
			throw texceptions::TYPEINCONGRUENCE;

		k = new variable(tresult::RES_BOOLEAN,NULL);
		if(p->gettype() == tresult::RES_BOOLEAN)
			k->setval(r->getbval() != p->getbval());
		else // real
			k->setval(r->getrval() != p->getrval());
	}
	return k;
}

variable* Parser::relexpr()
{
	// Real ist der numerische Standard-Typ bei booleeschen Ausdrücken
	variable* r = mathexpr(tresult::RES_REAL,true);
	variable* k = r;
	if(pLexer->look()->gettype() == '<') //<mathexpr> "<" <mathexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);
		k->setval(r->getrval() < mathexpr(tresult::RES_REAL,true)->getrval());
	}
	else if(pLexer->look()->gettype() == ttypes::LESS_EQUALS) //<mathexpr> "<=" <mathexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);
		k->setval(r->getrval() <= mathexpr(tresult::RES_REAL,true)->getrval());
	}
	else if(pLexer->look()->gettype() == '>') //<mathexpr> ">" <mathexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);	
		k->setval(r->getrval() > mathexpr(tresult::RES_REAL,true)->getrval());
	}
	else if(pLexer->look()->gettype() == ttypes::GREATER_EQUALS) //<mathexpr> ">=" <mathexpr>
	{
		pLexer->next();
		k = new variable(tresult::RES_BOOLEAN,NULL);
		k->setval(r->getrval() >= mathexpr(tresult::RES_REAL,true)->getrval());
	}
	return k;
}

variable* Parser::mathexpr(int numerictype,bool isboolexpr) // mathematischer Ausdruck
{
	variable *v = mathterm(numerictype,isboolexpr);
	// Verarbeitung von + und - Ketten
	while((pLexer->look()->gettype() == '+') || (pLexer->look()->gettype() == '-'))
	{
		if(pLexer->look()->gettype() == '+') // <mathexpr> "+" <mathterm>
		{
			pLexer->next();
			if(numerictype == tresult::RES_INTEGER)
				v->setval(v->getival() + mathterm(numerictype,isboolexpr)->getival());
			else if(numerictype == tresult::RES_REAL)
				v->setval(v->getrval() + mathterm(numerictype,isboolexpr)->getrval());
			else // Boolean
				throw texceptions::BOOLEANILLEGALLYFOUND;
		}
		else if(pLexer->look()->gettype() == '-') // <mathexpr> "-" <mathterm>
		{
			pLexer->next();
			if(numerictype == tresult::RES_INTEGER)
				v->setval(v->getival() - mathterm(numerictype,isboolexpr)->getival());
			else if(numerictype == tresult::RES_REAL)
				v->setval(v->getrval() - mathterm(numerictype,isboolexpr)->getrval());
			else // Boolean
				throw texceptions::BOOLEANILLEGALLYFOUND;
		}
	}
	return v;
}

variable* Parser::mathterm(int numerictype,bool isboolexpr)
{
	variable *v = mathunary(numerictype,isboolexpr);
	// Verarbeitung von * und / Ketten
	while((pLexer->look()->gettype() == '*') || (pLexer->look()->gettype() == '/'))
	{
		if(pLexer->look()->gettype() == '*')
		{
			pLexer->next();
			if(numerictype == tresult::RES_INTEGER)
				v->setval(v->getival() * mathunary(numerictype,isboolexpr)->getival());
			else if(numerictype == tresult::RES_REAL)
				v->setval(v->getrval() * mathunary(numerictype,isboolexpr)->getrval());
			else // Boolean
				throw texceptions::BOOLEANILLEGALLYFOUND;
		}
		else if(pLexer->look()->gettype() == '/')
		{
			pLexer->next();
			if(numerictype == tresult::RES_INTEGER)
				v->setval(static_cast<int>(v->getival() / mathunary(numerictype,isboolexpr)->getival()));
			else if(numerictype == tresult::RES_REAL)
				v->setval(static_cast<REALNUM>(v->getrval() / mathunary(numerictype,isboolexpr)->getrval()));
			else // Boolean
				throw texceptions::BOOLEANILLEGALLYFOUND;
		}
	}
	return v;
}

variable* Parser::mathunary(int numerictype,bool isboolexpr)
{
	// Verarbeitung von unärem Minus und booleschen Not
	if(pLexer->look()->gettype() == '-')
	{
		pLexer->next();
		variable* k = new variable(numerictype,NULL);
		if(numerictype == tresult::RES_INTEGER)
			k->setval(- mathunary(numerictype,isboolexpr)->getival());
		else if(numerictype == tresult::RES_REAL)
			k->setval(- mathunary(numerictype,isboolexpr)->getrval());
		else // Boolean
			throw texceptions::BOOLEANILLEGALLYFOUND;
		return k;
	}
	else if(pLexer->look()->gettype() == keywords::NOT)
	{	
		if(!isboolexpr)
			throw texceptions::BOOLEANOPERATORTYPEFAULT;
		pLexer->next();
		variable* n = new variable(tresult::RES_BOOLEAN,NULL);
		n->setval(!(mathunary(numerictype,true)->getbval()));
		return n;
	}
	return factor(numerictype,isboolexpr);
}

variable* Parser::factor(int numerictype,bool isboolexpr)
{
	switch(pLexer->look()->gettype())
	{
	case '(':
		{
			pLexer->next();
			variable* v;
			if(isboolexpr) 
				v = boolexpr(); // bei booleschem Ausdruck
			else
				v = mathexpr(numerictype,false); // bei mathematischem Ausdruck

			if(pLexer->look()->gettype() == ')')
				pLexer->next();
			else
				throw texceptions::CLOSEBRACKET_EXPECTED;
			return v;
			break;
		}
	case ttypes::INT: // integer-Literal
		{
			variable* c;
			if(numerictype == tresult::RES_REAL)
			{
				c = new variable(tresult::RES_REAL,NULL);
				c->setval(static_cast<REALNUM>(dynamic_cast<integer*>(pLexer->look())->getvalue()));
			}
			else // Integer
			{
				c = new variable(tresult::RES_INTEGER,NULL);
				c->setval(dynamic_cast<integer*>(pLexer->look())->getvalue());
			}
			pLexer->next();
			return c;
			break;
		}
	case ttypes::REALLIT: // real-Literal
		{
			variable* x;
			if(numerictype == tresult::RES_INTEGER)
			{
				x = new variable(tresult::RES_INTEGER,NULL);
				x->setval(static_cast<int>(dynamic_cast<real*>(pLexer->look())->getvalue()));
			}
			else // Boolean + Real
			{
				x = new variable(tresult::RES_REAL,NULL);
				x->setval(dynamic_cast<real*>(pLexer->look())->getvalue());
			}
			pLexer->next();
			return x;
			break;
		}
	case keywords::TRUE: // true Ausdruck
		{
			if(!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			variable* l = new variable(tresult::RES_BOOLEAN,NULL);
			l->setval(true);
			pLexer->next();
			return l;
			break;
		}
	case keywords::FALSE: // false Ausdruck
		{
			if(!isboolexpr)
				throw texceptions::BOOLEANILLEGALLYFOUND;
			variable* l = new variable(tresult::RES_BOOLEAN,NULL);
			l->setval(false);
			pLexer->next();
			return l;
			break;
		}
	case ttypes::ID: // Textsymbol gefunden
		{
			//Variable bestimmen
			std::list<variable*>::iterator iter;
			for(iter = variables.begin();iter != variables.end(); iter++)
			{
				if(strcmp(dynamic_cast<identifier*>(pLexer->look())->get(),(*iter)->getname())==0)
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
	// Begin beendet Variablendeklarationsblock
	while(pLexer->look()->gettype() != keywords::BEGIN)
	{
		decl();
		pLexer->next();
	}
}

void Parser::decl()
{
	token* t1 = pLexer->look();
	if(t1->gettype() == ttypes::ID) // Variablenname
	{
		pLexer->next();
		token* t2 = pLexer->look();
		if(t2->gettype() == ':') // erforderlicher Doppelpunkt
		{
			pLexer->next();
			token* t3 = pLexer->look();
			//Datentypüberprüfung
			if(t3->gettype() == keywords::INTEGER)
			{
				variables.push_back(new variable(tresult::RES_INTEGER,dynamic_cast<identifier*>(t1)->get()));
			}
			else if(t3->gettype() == keywords::REAL)
			{
				variables.push_back(new variable(tresult::RES_REAL,dynamic_cast<identifier*>(t1)->get()));
			}
			else if(t3->gettype() == keywords::BOOLEAN)
			{
				variables.push_back(new variable(tresult::RES_BOOLEAN,dynamic_cast<identifier*>(t1)->get()));
			}
		}
		pLexer->next(); // <;>
	}
}

} // namespace