#ifndef ARCHIVOINDICEPADRE_H_
#define ARCHIVOINDICEPADRE_H_

#include <iostream>
using namespace std;

#include "ArchivoRegistros.h"
#include "Nodo.h"

/*Previous Declaration*/
class Lista;
class Clave;
class Nodo;

/******************************************************************************/
/* Clase Archivo Indice - (Abstracta)*/
/*-----------------------------------*/

class ArchivoIndice{

protected:
	ArchivoRegistros* archivoIndex;
	/*Archivo de control de nodos liberados*/
	ArchivoRegistros* archivoLiberados;
	int tamanioNodo;
	
	/*Contadores de Lecturas y Escrituras*/
	int cantLecturas;
	int cantEscrituras;
	int cantLecturasControl;
	int cantEscriturasControl;
	
	/*Header del Nodo*/
	struct Header{
		int nivel;
		int espacioLibre;
		int hijoIzq;	
	}; 
	
	/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
	 * de escritura dependiendo si es un nodo hoja o un nodo no hoja*/
	virtual void copiarClaveHoja(Clave* clave,char* &puntero) = 0;
	
	virtual void copiarClaveNoHoja(Clave* clave,char* &puntero) = 0;
	
	/*Esta funcion se encarga de leer cada tipo de Clave de un buffer
	 * dependiendo si es un nodo hoja o un nodo no hoja*/
	virtual Clave* leerClaveHoja(char* &buffer) = 0;
	
	virtual Clave* leerClaveNoHoja(char* &buffer) = 0;
	
	/*Escribe una raiz vacia para cuando se crea el archivoIndice*/
	virtual void escribirRaizVacia();
	
	
public:
	/*Si el archivo esta vacio, crea una raiz vacia*/
	ArchivoIndice(int tamNodo,string nombreArchivo);
	
	/*Recibe un nodo creado y lo llena con la informacion leida (setea tamanioLibre/tamanioClave/posicionArchivo)*/
	virtual void leer(int numeroRegistro, Nodo* nodoLeido);
	
	/*PRE: El nodoLeido ya fue creado y contiene toda su informacion
	 * exepto su posicionEnArchivo*/
	virtual void grabarNuevoNodo(Nodo* nodoNuevo);
	
	/*Permite modificar la informacion de un nodo -> inclusive la raiz si posicion = 0*/
	virtual void sobreescribirNodo(Nodo* nodoModif);
	
	/*Agrega una referencia en el archivo de nodos liberados al
	 * nodo que se quiere eliminar*/
	virtual void eliminarNodo(int posicion);
	
	/*Exportar el archivo de Indice*/
	void exportar(ostream &archivoTexto,int posicion);
	
	
	/*Getters*/
	virtual int getTamanioNodo();
	int getCantLecturas();
	int getCantEscrituras();
	int getCantLecturasControl();
	int getCantEscriturasControl();
	static int getTamanioHeader();
	
	virtual ~ArchivoIndice();
};

/*******************************************************/
/* Clase Archivo de Indice Secundario                  */
/*-----------------------------------------------------*/

class ArchivoIndiceSecundario: public ArchivoIndice{
	
private:
		
	ArchivoRegistros* archivoLista;
	int tamanioArray;
	int cantLecturasLista;
	int cantEscriturasLista;
	
public:
	 
	/*Constructor*/
	ArchivoIndiceSecundario(int tamNodo,string nombreArchivo,int tamanioBloqueLista);
  	
	/*POST: Devuelve una lista de claves primaras de la posicion dada.
	 * RECORDAR: Borrar la lista una vez utilizada*/
	Lista* obtenerLstClavesP(int posicion);
	
	/*POST: SobreEscribe con listaClaves la lista que se encuentra en la posicion dada
	 * para actualizarla una vez insertada una nueva referencia en la lista*/
	void sobreEscribirListaClavesP(int posicion, Lista* listaClaves);
	
	/*POST: Graba una nueva lista de claves primarias en el archivo, devolviendo la posicion
	 * donde se grabo, 
	 * $$$: Recordar que el valor que sale de aca se tiene que grabar
	 * en el NODO, estoy hay que hacerlo antes de insertar en el arbol*/ 
	int grabarNuevaLstClavesP(Lista* listaClaves);
	
	virtual ~ArchivoIndiceSecundario();
	
	int getCantEscriturasLista();
	int getCantLecturasLista();
	Clave* leerClaveNoHoja(char* &buffer){return NULL;};
	Clave* leerClaveHoja(char* &buffer){return NULL;};
	void copiarClave(Clave* clave,char* &puntero){};
	void copiarClaveHoja(Clave* clave,char* &puntero){};
	/*Redefino el exportar para que tb imprima la lista de claves primarias*/
	void exportar(ostream &archivoTexto,int posicion);
};



#endif /*ARCHIVOINDICEPADRE_H_*/
