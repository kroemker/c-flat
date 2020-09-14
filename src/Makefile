LIB_NAME=cflat

CXX=g++
CXXFLAGS=-std=c++0x -I. -w
LIBS=
DEPS= exception.h function.h identifier.h Instruction.h integer.h Lexer.h Parser.h real.h stringlit.h symbols.h token.h variable.h
OBJ= exception.o function.o identifier.o Instruction.o integer.o Lexer.o Parser.o real.o stringlit.o symbols.o token.o variable.o

AR= ar rcu
RM= rm -f

CFLATLIB_WIN=$(LIB_NAME).lib
CFLATLIB_LINUX=$(LIB_NAME).a
CFLATEXE_WIN=$(LIB_NAME).exe
CFLATEXE_LINUX=$(LIB_NAME)

# %.o: %.cpp $(DEPS)
# 	$(CXX) -c -o $@ $< $(CXXFLAGS)

Windows: $(CFLATLIB_WIN) $(CFLATEXE_WIN)

Linux: $(CFLATLIB_LINUX) $(CFLATEXE_LINUX)

$(CFLATLIB_WIN) $(CFLATLIB_LINUX): $(OBJ)
	$(AR) $@ $(OBJ)

$(CFLATEXE_WIN) $(CFLATEXE_LINUX): $(OBJ) main.o
	$(CXX) -o $@ $(OBJ) main.o 

clean:
	$(RM) $(OBJ) $(CFLATLIB_WIN) $(CFLATLIB_LINUX) $(CFLATEXE_WIN) $(CFLATEXE_LINUX)

exception.o: exception.cpp exception.h symbols.h token.h
function.o: function.cpp function.h symbols.h
identifier.o: identifier.cpp symbols.h token.h identifier.h
Instruction.o: Instruction.cpp Instruction.h symbols.h
integer.o: integer.cpp integer.h token.h
Lexer.o: Lexer.cpp Lexer.h token.h real.h identifier.h integer.h stringlit.h
Parser.o: Parser.cpp Parser.h Instruction.h symbols.h Lexer.h function.h variable.h token.h exception.h
real.o: real.cpp real.h symbols.h token.h
stringlit.o: stringlit.cpp stringlit.h
symbols.o: symbols.h
token.o: token.cpp token.h symbols.h
variable.o: variable.cpp variable.h symbols.h

main.o: main.cpp symbols.h Lexer.h Parser.h exception.h