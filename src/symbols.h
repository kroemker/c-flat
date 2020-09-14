#pragma once

#include <cstring>

#define STACK_ENTRY_WIDTH	32
#define STACK_ENTRY_SIZE	(STACK_ENTRY_WIDTH / 8)

#define STACK_SIZE			0x400
#define STACK_NUM_ENTRIES	(STACK_SIZE / STACK_ENTRY_SIZE)

#define SP					stack[STACK_NUM_ENTRIES - 1].i
#define PC					stack[STACK_NUM_ENTRIES - 2].i
#define STACK(n)			stack[(SP + (n)) / STACK_ENTRY_SIZE]
#define GLOBAL(n)			stack[STACK_NUM_ENTRIES - ((n) / STACK_ENTRY_SIZE) - 1]
#define STACK_INBOUNDS(n)	(((SP + (n)) / STACK_ENTRY_SIZE) < STACK_NUM_ENTRIES)

#define ID_SIZE_LIMIT		64
#define TOKENTYPES			8
#define KEYWORDTYPES		512
#define KEYWORDCOUNT		23

#define SAFEDEL(p) if(p)delete p; p = NULL

namespace cflat
{
	union StackEntry
	{
		int i;
		float f;

		StackEntry() { memset(this, 0, sizeof(StackEntry)); }
		StackEntry(int i) { this->i = i; }
		StackEntry(float f) { this->f = f; }
	};

	typedef StackEntry Argument;
	typedef StackEntry* Stack;

	typedef void (*ExternalFunctionPtr)(Stack);

	enum TokenTypes
	{
		FLOAT_LITERAL = 256,
		INT_LITERAL,
		STRING,
		GREATER_EQUALS,
		LESS_EQUALS,
		UNEQUALS,
		EQUALS,
		ID,
	};

	extern const char* ttypes_s[];

	enum Keywords
	{
		VAR = 512,
		BEGIN,
		END,
		IF,
		ELSE,
		WHILE,
		DO,
		AND,
		OR,
		NOT,
		TRUE,
		FALSE,
		RETURN,
		// DataType
		T_U8, T_U16, T_U32,
		T_S8, T_S16, T_S32,
		T_F32,
		INT,
		FLOAT,
		FUNCTION,
	};

	extern const char* keywords_s[];

	enum exceptions
	{
		SYNTAX_ERROR,
		CLOSEBRACKET_EXPECTED,
		OPENBRACKET_EXPECTED,
		COMMA_EXPECTED,
		UNDEFINED_SYMBOL,
		SEMICOLON_EXPECTED,
		ID_EXPECTED,
		SYMBOL_REDEFINITION,
		INVALID_CHARACTER,
		DECLARATION_EXPECTED,
		TYPE_EXPECTED,
		NO_ENTRY_POINT,
		NOT_IMPLEMENTED,
	};

	extern const char* exceptions_s[];

	enum DataType
	{
		NO_TYPE,
		U8,
		U16,
		U32,
		S8,
		S16,
		S32,
		F32,
		FUNC
	};

	extern int typesizes[];

	enum Opcodes
	{
		NOP,
		// stack
		PUSH,
		POP,
		// data
		MW,
		MH,
		MB,
		MG,
		LDI,
		LDF,
		// int arithmetic
		ADD,
		SUB,
		MULT,
		DIV,
		MOD,
		// float arithmetic
		ADDF,
		SUBF,
		MULTF,
		DIVF,
		// bit
		BNOT,
		BOR,
		BAND,
		BXOR,
		// logicals
		LNOT,
		LAND,
		LOR,
		// relationals
		EQ,
		EQF,
		LT,
		LET,
		LTF,
		LETF,
		// jumps
		J,
		JT,
		JF,
		JR,
		CL,
		CLE,
		// conversion
		CTF,
		CTI,
		// end
		INIT,
		Q,
	};

	extern char* opcodes_s[];
	extern int opcodes_nargs[];
}