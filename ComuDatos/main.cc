/* Created by Anjuta version 1.2.4a */
/*	This file will not be overwritten */
#include "ComuDatos.h"
#include <iostream>

int main(int argc, char**argv)
{
	string a = argv[0];
	
	ComuDatos misComu(argv);

	misComu.ejecutable("./proceso2");
	
	misComu.agregarParametro("dos", 1);

	misComu.lanzar();
	
	misComu.escribir("Luke soy tu hijo");
	
	cout << b << "main lee: " << misComu.leer(100) << endl;
	
	misComu.liberarRecursos();
	
	return 0;
}
