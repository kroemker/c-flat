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

	static const char* ttypes_s[] =
	{
		"TYPE_REAL",
		"TYPE_INT",
		"TYPE_STRING",
		"TYPE_GREATER_EQUALS",
		"TYPE_LESS_EQUALS",
		"TYPE_UNEQUALS",
		"TYPE_EQUALS",
		"TYPE_ID"
	};

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

	static const char* keywords_s[] =
	{
		"var",
		"begin",
		"end",
		"if",
		"else",
		"while",
		"do",
		"and",
		"or",
		"not",
		"true",
		"false",
		"return",
		// DataType
		"u8", "u16", "u32",
		"s8", "s16", "s32",
		"f32",
		"int",
		"float",
		"function",
	};

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

	static const char* exceptions_s[] =
	{
		"Syntax Error",
		"')' expected",
		"'(' expected",
		"',' expected",
		"Undefined Symbol",
		"';' expected",
		"Identifier expected",
		"Redefinition of symbol",
		"Invalid character found",
		"Declaration expected",
		"Type expected",
		"Function 'main' was not found",
		"Requested feature is not yet implemented",
	};

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

	static int typesizes[] =
	{
		0,
		1,
		2,
		4,
		1,
		2,
		4,
		4
	};

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

	static char* opcodes_s[] =
	{
		"NOP",
		// stack
		"PUSH",
		"POP",
		// data
		"MW",
		"MH",
		"MB",
		"MG",
		"LDI",
		"LDF",
		// int arithmetic
		"ADD",
		"SUB",
		"MULT",
		"DIV",
		"MOD",
		// float arithmetic
		"ADDF",
		"SUBF",
		"MULTF",
		"DIVF",
		// bit
		"BNOT",
		"BOR",
		"BAND",
		"BXOR",
		// logical
		"LNOT",
		"LAND",
		"LOR",
		// relational
		"EQ",
		"EQF",
		"LT",
		"LET",
		"LTF",
		"LETF",
		// jump
		"J",
		"JT",
		"JF",
		"JR",
		"CL",
		"CLE",
		// conversion
		"CTF",
		"CTI",
		// start end
		"INIT",
		"Q",
	};

	static int opcodes_nargs[] =
	{
		0, // NOP
		1, 1, // stack
		3, 3, 3, 2, 3, 3, // data
		3, 3, 3, 3, 3, // int arithmetic
		3, 3, 3, 3, // float arithmetic
		2, 3, 3, 3, // bit
		3, 3, 3, // logical
		3, 3, 3, 3, 3, 3, // relational
		1, 2, 2, 0, 1, 1, // jump
		2, 2, // conversion
		0, 0
	};
}