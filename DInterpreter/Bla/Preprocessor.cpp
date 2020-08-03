#include "Preprocessor.h"

Preprocessor::Preprocessor(Lexer* p) : pLexer(p)
{
}

Preprocessor::~Preprocessor(void)
{
}

void Preprocessor::preprocess()
{
	pLexer->jumpto(0);
	token* t = pLexer->look();
	if(t->gettype() == keywords::VAR)
	{
		pLexer->next();
		decls();
	}
	pLexer->next();
	while(!pLexer->endofstream())
	{
		if(pLexer->look() == ttypes::ID)
		{
			std::list<variable*>::iterator iter;
			for(iter = variables.begin(); iter != variables.end(); iter++)
			{
				identifier* id = dynamic_cast<identifier*>(pLexer->look());
				if(strcmp((*iter)->name,id->tostring()) == 0)
				{
					id->makeempty();
					id->append('v');
					id->append(vartypes[(*iter)->type]);
					id->append(
					break;
				}
			}
		}
		pLexer->next();
	}
}

void Preprocessor::decls()
{
	while(pLexer->look()->gettype() != keywords::BEGIN)
	{
		decl();
		pLexer->next();
	}
}

void Preprocessor::decl()
{
	token* t1 = pLexer->look();
	if(t1->gettype() == ttypes::ID)
	{
		pLexer->next();
		token* t2 = pLexer->look();
		if(t2->gettype() == ':')
		{
			pLexer->next();
			token* t3 = pLexer->look();
			if(t3->gettype() == keywords::INTEGER)
			{
				variables.push_back(new variable(tresult::RES_INTEGER,variables.size,dynamic_cast<identifier*>(t3)->tostring()))
			}
			else if(t3->gettype() == keywords::REAL)
			{
				variables.push_back(new variable(tresult::RES_REAL,variables.size,dynamic_cast<identifier*>(t3)->tostring()))
			}
			else if(t3->gettype() == keywords::BOOLEAN)
			{
				variables.push_back(new variable(tresult::RES_BOOLEAN,variables.size,dynamic_cast<identifier*>(t3)->tostring()))
			}
		}
		pLexer->next(); // <;>
	}
}