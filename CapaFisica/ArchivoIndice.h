#ifndef ARCHIVOINDICE_H_
#define ARCHIVOINDICE_H_

#include "ArchivoIndicePadre.h"
#include "Clave.h"

/******************************************************************************/
/* Clase ArchivoIndice Codigo Producto */
/*-------------------------------------*/

class ArchivoIndiceEnteroGriego: public ArchivoIndice{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceEnteroGriego(int tamNodo,string nombreArchivo);
	
};


class ArchivoIndiceEnteroRomano: public ArchivoIndiceSecundario{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceEnteroRomano(int tamNodo,string nombreArchivo);
	
};


/******************************************************************************/
/* Clase ArchivoIndiceCadena - Indice Secundario                              */
/*----------------------------------------------------------------------------*/


class ArchivoIndiceVariableGriego: public ArchivoIndice{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceVariableGriego(int tamNodo,string nombreArchivo,int tamanioBloqueLista);
	
};

class ArchivoIndiceVariableRomano: public ArchivoIndiceSecundario{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceVariableRomano(int tamNodo,string nombreArchivo,int tamanioBloqueLista);
	
};


/********************************************************************************************/
/* Clase ArchivoIndice Nombre y Marca de Producto (Clave Candidata) - Indice Identificacion */
/*------------------------------------------------------------------------------------------*/


class ArchivoIndiceCompuestoGriego: public ArchivoIndice{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceCompuestoGriego(int tamNodo,string nombreArchivo);
	
};

class ArchivoIndiceCompuestoRomano: public ArchivoIndiceSecundario{
	
private:
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura*/
	void copiarClaveHoja(Clave* clave,char* &puntero);
	void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
	Clave* leerClaveHoja(char* &buffer);
	Clave* leerClaveNoHoja(char* &buffer);
	
public:
	ArchivoIndiceCompuestoRomano(int tamNodo,string nombreArchivo);
	
};



#endif /*ARCHIVOINDICE_H_*/
