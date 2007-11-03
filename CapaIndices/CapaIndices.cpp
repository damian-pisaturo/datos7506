//#include "CapaIndices.h"

#include <iostream>
#include "Bloque/Bloque.h"

using namespace std;
int main(int argc, char** argv) {
	
	//BStarTree bStarTree(1024);
	
	//cout << "Palurdos" << endl;
	//cout << "H" << endl; 
//	puts("Palurdos");
//	puts("H");
	
	int enteroUno = 1;
	int enteroDos = 320;
	char zeta = 'z';
	
	int enteroTres = 10;
	int enteroCuatro = 4;
	char ele = 'L'; 
		
	
	
	unsigned short espLibre = 37;
	unsigned short tamanio = 50;
	unsigned short cantRegs = 0; 
	unsigned short tamReg = 9;
	char *registro = new char [tamReg];
	
	RegisterInfo * listaLoca = new RegisterInfo();
	Bloque * bloque =  new Bloque(0,tamanio);
	memcpy(registro,&enteroUno,sizeof(int));
	memcpy(&registro[sizeof(int)],&zeta,sizeof(char));
	memcpy(&registro[sizeof(int) + sizeof(char)],&enteroDos,sizeof(int));
	bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	delete [] registro;
	memcpy(registro,&enteroTres,sizeof(int));
	memcpy(&registro[sizeof(int)],&ele,sizeof(char));
	memcpy(&registro[sizeof(int)+sizeof(char)],&enteroCuatro,sizeof(int));
	bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	int clave = 4;
	
	bloque->bajaRegistro(listaLoca->getParameterList(),&clave);
	delete []registro;
	char* datos = bloque->getDatos();
	memcpy(&espLibre,datos,sizeof(unsigned short));
	memcpy(&cantRegs,&datos[2],sizeof(unsigned short));
	memcpy(&enteroUno,&datos[4],sizeof(int));
	memcpy(&zeta,&datos[8],sizeof(char));
	memcpy(&enteroDos,&datos[9],sizeof(int));
	memcpy(&enteroTres,&datos[13],sizeof(int));
	memcpy(&ele,&datos[17],sizeof(char));
	memcpy(&enteroCuatro,&datos[18],sizeof(int));
	
	
	cout << "Espacio Libre"<<endl;
	cout << espLibre<<endl;
	cout <<"Cantidad de registros: "<<endl;
	cout <<cantRegs<<endl;
	cout <<"Primer dato del registro:"<<endl;
	cout <<enteroUno<<endl;
	cout <<"Segundo dato del registro:"<<endl;
	cout <<zeta<<endl;
	cout <<"Tercer dato del registro:"<<endl;
	cout <<enteroDos<<endl;

	cout <<"Primer dato del registro2:"<<endl;
	cout <<enteroTres<<endl;
	cout <<"Segundo dato del registro2:"<<endl;
	cout <<ele<<endl;
	cout <<"Tercer dato del registro2:"<<endl;
	cout <<enteroCuatro<<endl;

	
	exit(0);  
}

