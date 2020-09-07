#pragma once

namespace cflat
{

#define ID_SIZE_LIMIT	64
#define TOKENTYPES		8
#define KEYWORDTYPES	512
#define KEYWORDCOUNT	21

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
	};

	enum exceptions
	{
		ASSIGNORFUNCTIONCALL_EXPECTED = 0,
		CLOSEBRACKET_EXPECTED,
		UNDEFINED_SYMBOL,
		SEMICOLON_EXPECTED,
		BOOLEANILLEGALLYFOUND,
		TYPEINCONGRUENCE,
		BOOLEANOPERATORTYPEFAULT,
		THENEXPECTED,
		DOEXPECTED,
		ID_EXPECTED,
		SYMBOL_REDEFINITION,
		INVALID_CHARACTER
	};

	static const char* texceptions_s[] =
	{
		"ASSIGN ':=' Symbol oder Funktionsaufruf wurde erwartet, konnte aber nicht erkannt werden!",
		"CLOSEBRACKET ')' Symbol wurde erwartet, aber nicht gefunden!",
		"UNDEFINED SYMBOL; Ein Symbol wurde gefunden, konnte aber nicht aufgelöst werden!",
		"SEMICOLON ';' Symbol wurde erwartet, aber nicht gefunden!",
		"BOOLEAN ILLEGALLY FOUND; Booleescher Typ wurde gefunden, aber nicht erwartet!",
		"TYPEINCONGRUENCE; Typen sind nicht gleich und können nicht bearbeitet werden!",
		"BOOLEAN OPERATOR TYPE FAULT; Der Operator ist nur auf booleesche Typen anwendbar!",
		"THEN EXPECTED; Das THEN-Schlüsselwort wurde erwartet, aber nicht gefunden!",
		"DO EXPECTED; Das DO-Schlüsselwort wurde erwartet, aber nicht gefunden!",
		"Identifier expected",
		"Redefinition of symbol",
		"Invalid character found"
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
		F32
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
		1, 2, 2, // jump
		2, 2, // conversion
		0
	};
}