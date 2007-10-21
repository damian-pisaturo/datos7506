#include "Bloque.h"


Bloque::Bloque(int num,int tam)
{
	numero = num;
	tamanio = tam;
	datos = new char[tamanio];
}

Bloque::~Bloque()
{
	delete [] datos;
}

void Bloque::organizarBloque(){
	
}
/*
 * Inserta un nuevo registro dentro del bloque
 * Retorna OK (0) si la inserción fue exitosa, o FAIL (1) en caso contrario
 **/
int Bloque::altaRegistro(char *registro){
	int offsetEspLibre;
	unsigned short longReg;
	char *auxEspLibre = new char [5]; //Reservo 4 bytes para obtener el espacio libre
	char *auxLongReg = new char[3];
	
	memcpy(auxLongReg,registro,2); //Obtengo la longitud del registro
	auxLongReg[3]= '\0';
	longReg = atoi(auxLongReg);
	
	//Obtengo el offset al espacio libre
	memcpy(auxEspLibre,datos,4);
	//TODO: Ver lo del '\0'
	auxEspLibre[5]= '\0';
	offsetEspLibre = atoi(auxEspLibre);
	
	//Si el registro tiene espacio dentro del bloque realizo la inserción
	if(verificarEspacioDisponible(longReg,offsetEspLibre)){
		
		//Actualizo el offset al espacio libre, suponiendo que longReg incluye la longitud completa del registro
		//inclusive los 2 bytes que indican la longitud.
		offsetEspLibre = offsetEspLibre + longReg; 
		insertarRegistro(registro,offsetEspLibre);
		return OK;	
	}
	return FAIL;
}
	
/*
 * 
 **/
int Bloque::bajaRegistro(list <string>listaParam,int longReg){
	int offsetToPk = 0;

	
}

int Bloque::modificarRegistro(list <string>listaParam,int longReg){}

bool Bloque::verificarEspacioDisponible(int longReg,int offsetEspLibre){
	int espacioDisponible;
	
	espacioDisponible = tamanio - offsetEspLibre;
	if(espacioDisponible < longReg)
		return false;
	else
		return true;
}

void Bloque::insertarRegistro(char *registro, int nuevoOffsetEspLibre){}
