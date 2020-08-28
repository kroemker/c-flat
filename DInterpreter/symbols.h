#pragma once

namespace cscript
{

#define LOW_PRECISION
#ifdef LOW_PRECISION
	#define REALNUM float
#else
	#define REALNUM double
#endif

#define ID_SIZE_LIMIT	64
#define TOKENTYPES		8
#define KEYWORDTYPES	512
#define KEYWORDCOUNT	16

#define SAFEDEL(p) if(p)delete p; p = NULL 

enum ttypes
{
	REALLIT=256,
	INT,
	STRING,
	GREATER_EQUALS,
	LESS_EQUALS,
	UNEQUALS,
	EQUALS,
	ID
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

enum keywords 
{
	VAR=512,
	BEGIN,
	END,
	IF,
	THEN,
	ELSE,
	WHILE,
	DO,
	AND,
	OR, 
	NOT,
	BOOLEAN,
	INTEGER,
	REAL,
	TRUE,
	FALSE
}; 

static const char* keywords_s[] =
{
	"var",
	"begin",
	"end",
	"if",
	"then",
	"else",
	"while",
	"do",
	"and",
	"or",
	"not",
    "boolean",
	"integer",
	"real",
	"true",
	"false"
};

enum texceptions 
{
	ASSIGNORFUNCTIONCALL_EXPECTED=0,
	CLOSEBRACKET_EXPECTED,
	UNDEFINED_SYMBOL,
	SEMICOLON_EXPECTED,
	BOOLEANILLEGALLYFOUND,
	TYPEINCONGRUENCE,
	BOOLEANOPERATORTYPEFAULT,
	THENEXPECTED,
	DOEXPECTED
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
	"DO EXPECTED; Das DO-Schlüsselwort wurde erwartet, aber nicht gefunden!"
};

enum tresult
{
	RES_BOOLEAN=0,
	RES_INTEGER,
	RES_REAL
};

}