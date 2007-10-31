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

#include "Clave.h"
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
		
		virtual ~BPlusTree() {}
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	    /*Insertar clave en un arbol B+*/	
		void insertarClave(Clave* clave);
		
		/*Eliminar clave pasada por parametro en un arbol B+*/
		void eliminarClave(Clave* clave);
	    
	  	/*Devuelve una copia de la clave buscada con su referencia 
	  	 * o sino la mas cercana (menor).
	  	 * Si el arbol esta vacio devuelve NULL.*/
		Clave* buscarClave(Clave* clave);	
		
		/* primero()
		 * Deja en nodoActual el primer nodo del set secuencial
		 * (menor o mayor, dependiendo del orden de insercion).
		 */
		void primero();
		
		Clave* siguiente();
		
	private:
		//////////////////////////////////////////////////////////////////////
		// Metodos privados
		//////////////////////////////////////////////////////////////////////       
    
		/*Devuelve un nodo con la raiz del arbol*/
		NodoBPlus* getRaiz();
		
		void insertarInterno(Nodo* actual,Clave* &clave,Codigo* &codigo,Nodo* &anterior);		
    
	    void eliminarInterno(Nodo* actual,Clave* clave,Codigo* codigo,Nodo* &nodoUnderflow);  
	    
	    void buscarInterno(Clave* clave,Clave*& claveBuscada);
	    
	    bool chequearEspacio(Nodo* otroNodo,Nodo* nodoUnderflow);
	    
	  	bool puedeDonar(Nodo* nodoDonador, Nodo* nodoAceptor);
    
	  	void split(Nodo* donador, Nodo* aceptor);
	  	
}; //Fin clase BPlusTree.

#endif /*BPLUSTREE_H_*/
