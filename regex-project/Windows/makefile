#nmake
program = nfa+tt.exe
CC = cl
FLAGS = /std:c++17 /EHsc /W4 /source-charset:utf-8 /O2 /Fe:$(program)

all: $(program)

$(program): nfa_plus_ttable.cpp regex.h
	$(CC) $(FLAGS) nfa_plus_ttable.cpp

clean:
	del /Q nfa_plus_ttable.obj nfa+tt.exe
