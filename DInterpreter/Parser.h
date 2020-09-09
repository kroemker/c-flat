#pragma once

#include <map>
#include <list>
#include "Instruction.h"
#include "symbols.h"
#include "Lexer.h"
#include "function.h"
#include "variable.h"
#include "token.h"
#include "exception.h"

namespace cflat
{

	class Parser
	{
	public:
		Parser(Lexer* lexer);
		~Parser(void);

		Exception*	parsenext();

		std::vector<Instruction>	instructions;
	private:
		DataType					boolexpr(int stackSlot);
		DataType					joinexpr(int stackSlot);
		DataType					equalityexpr(int stackSlot);
		DataType					relexpr(int stackSlot);
		DataType					mathexpr(int stackSlot);
		DataType					mathterm(int stackSlot);
		DataType					mathunary(int stackSlot);
		DataType					factor(int stackSlot);
		DataType					generalizedTypecast(DataType t, DataType s, int tSlot, int sSlot);
		void						forcedTypecast(DataType t, DataType s, int tSlot);
		void						stmts();
		DataType					decl(bool allowFuncs);
		DataType					datatype();
		void						decls(bool allowFuncs);
		void						functionCall(char* name);
		void						removeAllNonGlobals();
		Variable*					getVariable(char * name);
		Function*					getFunction(char * name);

		std::vector<Function>		functions;
		std::vector<Variable>		variables;
		Lexer*						lexer;
		int							globalStackSize;
		int							localStackSize;
		int							maxLocalStackSize;
	};

}