#ifndef BPTREE_H_
#define BPTREE_H_

#ifndef NULL
#define NULL 0
#endif

#include "Clave.h"
#include "Nodo.h"
#include "ArchivoIndicePadre.h"

#include <string>
#include <iostream>
using namespace std;

class ArchivoIndice;
class Nodo;
class Clave;

/******************************************************************************/
/* Clase Codigo */
/*--------------*/

class Codigo{
private:
	string valor;

public:
	Codigo();
	string getValor();
	void setValor(string val);
};

/******************************************************************************/
/* Clase ARBOL B+*/
/*---------------*/

class BpTree{

private:
	
	Nodo* nodoActual;
	ArchivoIndice* archivoIndice;           
    
    void insertarInterno(Nodo* Actual,Clave* &clave,Codigo* &codigo,Nodo* &Anterior);
    
    Nodo* getRaiz();
    
    void eliminarInterno(Nodo* Actual,Clave* clave,Codigo* codigo,Nodo* &nodoConSubflow);  
    
    void buscarInterno(Clave* clave,Clave*& Buscada);
    
    bool chequearEspacio(Nodo* otroNodo,Nodo* nodoConSubflow);
    
  	bool puedeDonar(Nodo* nodoDonador, Nodo* nodoAceptor);
    
    void Split(Nodo* donador, Nodo* aceptor);
    
public:


	BpTree(ArchivoIndice* archivo);

    /*Inserción*/	
	void Insertar(Clave* clave);
	
	/*Eliminación*/
	void Eliminar(Clave* clave);
    
  	/*Devuelve una copia de la clave buscada con su referencia 
  	 * o sino la mas cercana (menor)
  	 * Si el arbol esta vacio devuelve NULL*/
	Clave* buscarClave(Clave* clave);	
	
	void primero();
	
	Clave* siguiente();
	         
	virtual ~BpTree();

};

#endif /*BPTREE_H_*/
