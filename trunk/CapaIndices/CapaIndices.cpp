//#include "CapaIndices.h"

#include <iostream>
#include "Bloque/Bloque.h"

using namespace std;

void cargarDefiniciones() {
	
	
	
	
}

int main(int argc, char** argv) {
	  
	
	cargarDefiniciones();
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
	
	// Pone los datos en el registro.
	memcpy(registro,&longReg,sizeof(unsigned short));
	memcpy(&registro[sizeof(unsigned short)],&enteroUno,sizeof(int));
	memcpy(&registro[sizeof(unsigned short)+sizeof(int)],&longString,sizeof(unsigned short));
	memcpy(&registro[sizeof(int)+2*sizeof(unsigned short)],&zeta,5*sizeof(char));
	memcpy(&registro[sizeof(int)+2*sizeof(unsigned short) + 5*sizeof(char)],&enteroDos,sizeof(int));
	int result = bloque->altaRegistro(listaLoca->getParameterList(),registro);
	
	cout<<"Alta registro 1:"<<endl;
	cout<<result<<endl;
	
	memcpy(registro2,&longReg2,sizeof(unsigned short));
	memcpy(&registro2[sizeof(unsigned short)],&enteroTres,sizeof(int));
	memcpy(&registro2[sizeof(unsigned short)+sizeof(int)],&longString2,sizeof(unsigned short));
	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short)],&lau,6*sizeof(char));
	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short) + 6*sizeof(char)],&enteroCuatro,sizeof(int));
		
	result = bloque->altaRegistro(listaLoca->getParameterList(),registro2);
		
//	lau = "abcdef";
	
//	memcpy(&registro2[sizeof(int)+2*sizeof(unsigned short)],&lau,6*sizeof(char));
	
	
	cout<<"Alta registro 2:"<<endl;
	cout<<result<<endl;
	
	void** clave = new void*[1];
	clave[0] = zeta;
	
	//result = bloque->modificarRegistro(listaLoca->getParameterList(),16,clave,registro2);
	cout<<"Modificar:"<<endl;
		cout<<result<<endl;
//	int clave  = 320;
	result = bloque->bajaRegistro(listaLoca->getParameterList(),clave);
	
	
	char* datos = bloque->getDatos(); 
	memcpy(&espLibre,datos,sizeof(unsigned short));
	memcpy(&cantRegs,&datos[2],sizeof(unsigned short));
	memcpy(&enteroUno,&datos[6],sizeof(int));
	memcpy(&lau,&datos[12],6*sizeof(char));
	memcpy(&enteroDos,&datos[18],sizeof(int));
	
	//Reistro 2
/*	memcpy(&enteroTres,&datos[23],sizeof(int));
	memcpy(&lau,&datos[29],6*sizeof(char));
	memcpy(&enteroCuatro,&datos[35],sizeof(int));
	*/
	//Reistro 2
/*		memcpy(&enteroTres,&datos[23],sizeof(int));
		memcpy(&lau,&datos[29],6*sizeof(char));
		memcpy(&enteroCuatro,&datos[35],sizeof(int));
*/	cout << "Elimino?"<<endl;
	cout << result<<endl;
	cout << "Espacio Libre"<<endl;
 	cout << espLibre<<endl;
	cout <<"Cantidad de registros: "<<endl;
	cout <<cantRegs<<endl;
	cout <<"Primer dato del registro:"<<endl;
	cout <<enteroUno<<endl;
	cout <<"Segundo dato del registro:"<<endl;
	cout <<lau<<endl;
	cout <<"Tercer dato del registro:"<<endl;
	cout <<enteroDos<<endl;
	
/*	cout <<"Primer dato del registro2:"<<endl;
		cout <<enteroTres<<endl;
		cout <<"Segundo dato del registro2:"<<endl;
		cout <<lau<<endl;
		cout <<"Tercer dato del registro2:"<<endl;
		cout <<enteroCuatro<<endl;
		*/
}

