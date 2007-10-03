#include "ComuDatos.h"
#include <iostream>

int main(int argc, char **argv)
{
	int cont;
	string a = argv[0];
	ComuDatos misComu(argv);
	
	if (argc > 1){
		cont = 0;
		while((misComu.parametro(cont) != FIN_PARAMETROS) and (cont < 100))
		{
			cout << "parámetros proceso2 "  << misComu.parametro(cont) << " - "<< cont << endl;
			cont++;
		}
		
		
		misComu.escribir("Luke soy tu padre");	
		cout << "proceso2 lee: " << misComu.leer(100) <<  endl;
	}

}
