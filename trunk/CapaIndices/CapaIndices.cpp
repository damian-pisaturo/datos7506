//#include "CapaIndices.h"

#include <iostream>
#include <map>
#include "Bloque/Bloque.h"

using namespace std;

typedef map<string, ListaNodos*> MapaDefiniciones;

void cargarDefiniciones(MapaDefiniciones &mapaDef) {
	
	ListaNodos* listaNodos = NULL;
	nodoLista nodo;
	
	//CARGO LAS DEFINICIONES DEL TIPO "PERSONA"
	
	listaNodos = new ListaNodos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	nodo.pk = "0";
	nodo.cantClaves = 1;
	
	listaNodos->push_back(nodo);
	
	//DNI
	nodo.tipo = TipoDatos::TIPO_ENTERO;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Nombre
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	//FechaNacimiento
	nodo.tipo = TipoDatos::TIPO_FECHA;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	mapaDef["Persona"] = listaNodos;
	
	
	//CARGO LAS DEFINICIONES DEL TIPO "PELICULA"

	listaNodos = new ListaNodos();
	
	//El primer nodo de la lista indica si el registro es variable o no.
	nodo.tipo = TipoDatos::TIPO_FIJO;
	nodo.pk = "34";
	nodo.cantClaves = 2;
	
	listaNodos->push_back(nodo);
	
	//Titulo (cadena de 15 carecteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Director (cadena de 10 caracteres)
	nodo.tipo = TipoDatos::TIPO_STRING;
	nodo.pk = "true";
	listaNodos->push_back(nodo);
	
	//Genero
	nodo.tipo = TipoDatos::TIPO_CHAR;
	nodo.pk = "false";
	listaNodos->push_back(nodo);
	
	mapaDef["Pelicula"] = listaNodos;
	
}

int main(int argc, char** argv) {
	 
	MapaDefiniciones mapaDef;
	
	cargarDefiniciones(mapaDef);
	/*
	 * Este codigo es para probar ABM con registros fijos.
	 * Cabe aclarar que bloque no es el encargado de chequear las claves repetidas, esa funcionalidad 
	 * esta en la clase bucket.
	 **/
/*
	int enteroUno = 1;
	int enteroDos = 320;
	char zeta = 'z';
	
	int enteroTres = 10;
	int enteroCuatro = 4;
	char ele = 'L'; 
		
	
	
	unsigned short espLibre = 37;
	unsigned short tamanio = 15;
	unsigned short cantRegs = 0; 
	unsigned short tamReg = 9;
	char *registro = new char [tamReg];
	
	RegisterInfo * listaLoca = new RegisterInfo();
	Bloque * bloque =  new Bloque(0,tamanio);
	memcpy(registro,&enteroUno,sizeof(int));
	memcpy(&registro[sizeof(int)],&zeta,sizeof(char));
	memcpy(&registro[sizeof(int) + sizeof(char)],&enteroDos,sizeof(int));
	bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	memcpy(registro,&enteroTres,sizeof(int));
	memcpy(&registro[sizeof(int)],&ele,sizeof(char));
	memcpy(&registro[sizeof(int)+sizeof(char)],&enteroCuatro,sizeof(int));
	int result = bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	enteroTres = 5;
	ele = 'h';
	enteroCuatro = 4;
	
	memcpy(registro,&enteroTres,sizeof(int));
	memcpy(&registro[sizeof(int)],&ele,sizeof(char));
	memcpy(&registro[sizeof(int)+sizeof(char)],&enteroCuatro,sizeof(int));
	
	
	int clave = 320;
	int result = bloque->modificarRegistro(listaLoca->getParameterList(),9,&clave,registro);
	//int result = bloque->bajaRegistro(listaLoca->getParameterList(),&clave);
	 */
/*	cout<< result<<endl;
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
	
	*/
	
	int enteroUno = 1;
	char *zeta = new char[5];
	zeta = "HOLAS";
	int enteroDos = 320;
	int enteroTres = 24;
	char *lau = new char [6];
	lau = "mlaura";
	int enteroCuatro = 321;
		
	unsigned short espLibre;
	unsigned short tamanio = 50;
	unsigned short cantRegs = 0; 
	char *registro = new char [17];
	char *registro2 = new char [18];
	unsigned short longReg = 15;
	unsigned short longReg2 = 16;
	unsigned short longString = 5;
	unsigned short longString2 = 6;
	RegisterInfo * listaLoca = new RegisterInfo();
	Bloque * bloque =  new Bloque(0,tamanio);
	memcpy(registro,&longReg,sizeof(unsigned short));
	memcpy(&registro[sizeof(unsigned short)],&enteroUno,sizeof(int));
	memcpy(&registro[sizeof(unsigned short)+sizeof(int)],&longString,sizeof(unsigned short));
	memcpy(&registro[sizeof(int)+2*sizeof(unsigned short)],&zeta,5*sizeof(char));
	memcpy(&registro[sizeof(int)+2*sizeof(unsigned short) + 5*sizeof(char)],&enteroDos,sizeof(int));
	int result = bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	cout<<"result 1:"<<endl;
	cout<<result<<endl;
	
	memcpy(registro2,&longReg2,sizeof(unsigned short));
	memcpy(&registro2[sizeof(unsigned short)],&enteroTres,sizeof(int));
	memcpy(&registro2[sizeof(unsigned short)+sizeof(int)],&longString2,sizeof(unsigned short));
	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short)],&lau,6*sizeof(char));
	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short) + 6*sizeof(char)],&enteroCuatro,sizeof(int));
		
	result = bloque->altaRegistro(listaLoca->getParameterList(),registro2);
		
	lau = "abcdef";
	
	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short)],&lau,6*sizeof(char));
	
	
	cout<<"result 2:"<<endl;
	cout<<result<<endl;
	result = bloque->modificarRegistro(listaLoca->getParameterList(),16,&enteroCuatro,registro2);
	cout<<"result 3:"<<endl;
		cout<<result<<endl;
//	int clave  = 320;
	//result = bloque->bajaRegistro(listaLoca->getParameterList(),&clave);
	
	
	char* datos = bloque->getDatos(); 
	memcpy(&espLibre,datos,sizeof(unsigned short));
	memcpy(&cantRegs,&datos[2],sizeof(unsigned short));
	memcpy(&enteroUno,&datos[6],sizeof(int));
	memcpy(&zeta,&datos[12],5*sizeof(char));
	memcpy(&enteroDos,&datos[17],sizeof(int));
	
	//Reistro 2
	memcpy(&enteroTres,&datos[23],sizeof(int));
	memcpy(&lau,&datos[29],6*sizeof(char));
	memcpy(&enteroCuatro,&datos[35],sizeof(int));
	
	//Reistro 2
		memcpy(&enteroTres,&datos[23],sizeof(int));
		memcpy(&lau,&datos[29],6*sizeof(char));
		memcpy(&enteroCuatro,&datos[35],sizeof(int));
	cout << "Elimino?"<<endl;
	cout << result<<endl;
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
		cout <<lau<<endl;
		cout <<"Tercer dato del registro2:"<<endl;
		cout <<enteroCuatro<<endl;
}

