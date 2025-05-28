#GNU make
program = nfa+tt
CC = g++
FLAGS = -std=c++17 -pthread -Wall -o2

all: $(program)

$(program): nfa_plus_ttable.cpp regex.h
	$(CC) $(FLAGS) -o2 -o $(program) nfa_plus_ttable.cpp
	chmod +rx menu.sh 1.sh 2.sh

clean:
	rm -f $(program)
