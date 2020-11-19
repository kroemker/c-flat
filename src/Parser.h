#pragma once

#include <map>
#include <list>

#include "Instruction.h"
#include "Symbols.h"
#include "Lexer.h"
#include "Function.h"
#include "Variable.h"
#include "Token.h"
#include "Exception.h"

namespace cflat
{
	class Parser
	{
	public:
		Parser(Lexer* lexer);
		~Parser(void);

		void registerExternalFunction(char* name, ExternalFunctionPtr f);
		int setEntryPoint(char* functionName);

		void parse();
		Exception* parsenext();
		char* getErrorString() { return errorString; }
		bool isError() { return error; }
		int getGlobalStackSize() { return globalStackSize; }

		Variable*					getVariable(char * name);
		Function*					getFunction(char * name);

		std::vector<Instruction>	instructions;
	private:
		DataType					assignexpr(int stackSlot);
		DataType					boolexpr(int stackSlot);
		DataType					joinexpr(int stackSlot);
		DataType					bitorexpr(int stackSlot);
		DataType					bitxorexpr(int stackSlot);
		DataType					bitandexpr(int stackSlot);
		DataType					equalityexpr(int stackSlot);
		DataType					relexpr(int stackSlot);
		DataType 					shiftexpr(int stackSlot);
		DataType					mathexpr(int stackSlot);
		DataType					mathterm(int stackSlot);
		DataType					mathunary(int stackSlot);
		DataType					memexpr(int stackSlot, Variable** outvar);
		DataType					factor(int stackSlot);
		DataType					generalizedTypecast(DataType t, DataType s, int tSlot, int sSlot);
		void						forcedTypecast(DataType t, DataType s, int tSlot);
		void						stmts();
		DataType					globaldecl();
		DataType					datatype();
		int							functionCall(char* name);
		void						loadVarToStack(int stackSlot, Variable* v);
		void						removeAllNonGlobals();

		Lexer*						lexer;
		char						errorString[128];
		bool						error;
		std::vector<Function>		functions;
		std::vector<Variable>		variables;
		int							globalStackSize;
		int							localStackSize;
		int							maxLocalStackSize;
		int							placeholderStack;
		Function*					entryPoint;
	};

}