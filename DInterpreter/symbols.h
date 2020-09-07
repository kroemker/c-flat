#pragma once

namespace cflat
{

#define ID_SIZE_LIMIT	64
#define TOKENTYPES		8
#define KEYWORDTYPES	512
#define KEYWORDCOUNT	22

#define SAFEDEL(p) if(p)delete p; p = NULL

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
		SYNTAX_ERROR = 0,
		CLOSEBRACKET_EXPECTED,
		OPENBRACKET_EXPECTED,
		UNDEFINED_SYMBOL,
		SEMICOLON_EXPECTED,
		ID_EXPECTED,
		SYMBOL_REDEFINITION,
		INVALID_CHARACTER,
		DECLARATION_EXPECTED
	};

	static const char* exceptions_s[] =
	{
		"Syntax Error",
		"')' expected",
		"'(' expected",
		"Undefined Symbol",
		"';' expected",
		"Identifier expected",
		"Redefinition of symbol",
		"Invalid character found",
		"Declaration expected"
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
		CL,
		CLE,
		// conversion
		CTF,
		CTI,
		// end
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
		"CL",
		"CLE",
		// conversion
		"CTF",
		"CTI",
		// end
		"Q",
	};

	static int opcodes_nargs[] =
	{
		0, // NOP
		1, 1, // stack
		2, 2, 2, 2, 2, // data
		3, 3, 3, 3, 3, // int arithmetic
		3, 3, 3, 3, // float arithmetic
		2, 3, 3, 3, // bit
		3, 3, 3, // logical
		3, 3, 3, 3, 3, 3, // relational
		1, 2, 2, 1, 1, // jump
		2, 2, // conversion
		0
	};
}