///////////////////////////////////////////////////////////////////////////
//	Archivo   : BPlusTree.h
//  Namespace : CapaIndice
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase BPlusTree.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura
///////////////////////////////////////////////////////////////////////////

#ifndef BPLUSTREE_H_
#define BPLUSTREE_H_

#include "../Common/Clave.h"
#include "NodoBPlus.h"
//#include "ArchivoIndicePadre.h"

#include <string>
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BPlusTree (Implementa Arboles B+ en disco)
///////////////////////////////////////////////////////////////////////////

class BPlusTree
{
	private:
	//////////////////////////////////////////////////////////////////////
	//Atributos privados
	//////////////////////////////////////////////////////////////////////
		NodoBPlus* nodoRaiz;
		NodoBPlus* nodoActual;
		unsigned short tamanioNodo;
		//ArchivoIndice* archivoIndice;   

	public:
	//////////////////////////////////////////////////////////////////////
	// Contructor/Destructor
	//////////////////////////////////////////////////////////////////////
		BPlusTree(unsigned short tamanioNodo);
		
		virtual ~BPlusTree();
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	    /*Insertar clave en un arbol B+*/	
		void insertar(Clave* clave);
		
		/*Eliminar clave pasada por parametro en un arbol B+*/
		bool eliminar(Clave* clave);
	    
	  	/*Devuelve una copia de la clave buscada con su referencia 
	  	 * o sino la mas cercana (menor).
	  	 * Si el arbol esta vacio devuelve NULL.*/
		Clave* buscar(Clave* clave) const;
		
		//Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario, la reemplaza por claveNueva
		//y devuelve true.
		bool modificar(Clave* claveVieja, Clave* claveNueva);
		
		//Deja en nodoActual el primer nodo del set secuencial
		void primero();
		
		Clave* siguiente();
		
	private:
		//////////////////////////////////////////////////////////////////////
		// Metodos privados
		//////////////////////////////////////////////////////////////////////       
    
		/*Devuelve un nodo con la raiz del arbol*/
		NodoBPlus* getRaiz() const;
		
		void insertarInterno(NodoBPlus* &nodoDestino, char* codigo);		
    
	    void eliminarInterno(NodoBPlus* nodoTarget, char* codigo);  
	    
	    NodoBPlus* buscarPadre(NodoBPlus* padre, NodoBPlus* hijo) const;
	    
	    NodoBPlus* buscarLugar(Clave* clave) const;
	    
	    NodoBPlus* buscarLugarRecursivo(NodoBPlus* nodo, Clave* clave) const;
	  	
}; //Fin clase BPlusTree.

#endif /*BPLUSTREE_H_*/
