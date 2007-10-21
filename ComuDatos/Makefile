all: proceso2 comudatos

comudatos: ComuDatos.o main.cc
	g++ -Wall -ansi -pedantic-errors -ggdb -DDEBUG ComuDatos.o main.cc -o comudatos

proceso2: ComuDatos.o proceso2.cc
	g++ -Wall -ansi -pedantic-errors -ggdb -DDEBUG ComuDatos.o proceso2.cc -o proceso2

ComuDatos.o: ComuDatos.cpp ComuDatos.h
	g++ -Wall -ansi -ggdb -DDEBUG ComuDatos.cpp -c

clean: 
	rm *.o comudatos proceso2 ComuDatos.o
