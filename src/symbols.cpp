#include "symbols.h"

namespace cflat
{
    const char* ttypes_s[] =
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

    const char* keywords_s[] =
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

    const char* exceptions_s[] =
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
		"Invalid entry point set",
		"Requested feature unavailable",
	};

    int typesizes[] =
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

    char* opcodes_s[] =
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
		// control
		"INIT",
		"Q",
		"YLD",
	};

	int opcodes_nargs[] =
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
		0, 0, 0, // control
	};
}
