all: proceso2 comudatos

comudatos: ComuDatos.o main.cc
	g++ ComuDatos.o main.cc -o comudatos

proceso2: ComuDatos.o proceso2.cc
	g++ ComuDatos.o proceso2.cc -o proceso2

ComuDatos.o: ComuDatos.cpp ComuDatos.h
	g++ ComuDatos.cpp -c

clean: 
	rm *.o comudatos proceso2 ComuDatos.o
