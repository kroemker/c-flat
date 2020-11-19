LIB_NAME=cflat

CXX=g++
CXXFLAGS=-std=c++0x -I.
OBJ= cflat.o Exception.o Function.o Identifier.o Instruction.o Integer.o Lexer.o Parser.o Optimizer.o Float.o StringLiteral.o Symbols.o Token.o Variable.o

AR= ar rcu
RM= rm -f

CFLATLIB_WIN=$(LIB_NAME).lib
CFLATLIB_LINUX=lib$(LIB_NAME).a
CFLATEXE_WIN=$(LIB_NAME).exe
CFLATEXE_LINUX=$(LIB_NAME)

Windows: $(CFLATLIB_WIN) $(CFLATEXE_WIN)

Linux: $(CFLATLIB_LINUX) $(CFLATEXE_LINUX)

$(CFLATLIB_WIN) $(CFLATLIB_LINUX): $(OBJ)
	$(AR) $@ $(OBJ)

$(CFLATEXE_WIN) $(CFLATEXE_LINUX): icflat.o
	$(CXX) -o $@ icflat.o -L. -l$(LIB_NAME)

clean:
	$(RM) icflat.o $(OBJ) $(CFLATLIB_WIN) $(CFLATLIB_LINUX) $(CFLATEXE_WIN) $(CFLATEXE_LINUX)

Exception.o: Exception.cpp Exception.h Symbols.h Token.h
Function.o: Function.cpp Function.h Symbols.h
Identifier.o: Identifier.cpp Symbols.h Token.h Identifier.h
Instruction.o: Instruction.cpp Instruction.h Symbols.h
Integer.o: Integer.cpp Integer.h Token.h
Lexer.o: Lexer.cpp Lexer.h Token.h Float.h Identifier.h Integer.h StringLiteral.h
Parser.o: Parser.cpp Parser.h Instruction.h Symbols.h Lexer.h Function.h Variable.h Token.h Exception.h
Optimizer.o: Optimizer.cpp Optimizer.h Parser.h
Float.o: Float.cpp Float.h Symbols.h Token.h
StringLiteral.o: StringLiteral.cpp StringLiteral.h
Symbols.o: Symbols.h
Token.o: Token.cpp Token.h Symbols.h
Variable.o: Variable.cpp Variable.h Symbols.h
cflat.o: cflat.cpp cflat.h Symbols.h Function.h Lexer.h Parser.h Instruction.h

icflat.o: icflat.cpp cflat.h