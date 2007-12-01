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

#include "../BTree/BTree.h"
#include "NodoBPlus.h"


#include <string>
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BPlusTree (Implementa Arboles B+ en disco)
///////////////////////////////////////////////////////////////////////////

class BPlusTree : public BTree
{
	private:
	//////////////////////////////////////////////////////////////////////
	//Atributos privados
	//////////////////////////////////////////////////////////////////////
		NodoBPlus* nodoActual;
		SetClaves::iterator iterClavesActual;

		NodoBPlus* nodoRaiz;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Contructor/Destructor
	//////////////////////////////////////////////////////////////////////
		BPlusTree(IndiceManager& indiceManager, unsigned short tamanioNodo);
		
		virtual ~BPlusTree();
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	    /*Insertar clave en un arbol B+*/	
		bool insertar(Clave* clave);
		
		/*Eliminar clave pasada por parametro en un arbol B+*/
		bool eliminar(Clave* clave);
	    
	  	/*Devuelve una copia de la clave buscada con su referencia 
	  	 * o sino la mas cercana (menor).
	  	 * Si el arbol esta vacio devuelve NULL.*/
		Clave* buscar(Clave* clave) const;
		
		//Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario, la reemplaza por claveNueva
		//y devuelve true.
		int modificar(Clave* claveVieja, Clave* claveNueva);
		
		//Deja en nodoActual el primer nodo del set secuencial y al iterClavesActual en el primer elemento dentro de ese nodo.
		void primero();
		
		//Deja en nodoActual el nodo del set secuencial donde se encuentre la primer clave mayor o igual a la recibida por parámetro
		//y al iterClavesActual en dicho elemento.
		void mayorOIgual(Clave* clave);
		
		//Deja en nodoActual el nodo del set secuencial donde se encuentre la primer clave mayor a la recibida por parámetro
		//y al iterClavesActual en dicho elemento.
		void mayor(Clave* clave);
		
		//Devuelve la siguiente clave dentro del árbol. DEBE ser llamado después de primero(), sin
		//modificaciones dentro del árbol luego de ser llamado este método, para garantizar su buen
		//funcionamiento. Si no hay más elementos siguientes devuelve NULL.
		Clave* siguiente();
		
		bool vacio() const { return (this->nodoRaiz == NULL); }
		
	private:
		//////////////////////////////////////////////////////////////////////
		// Metodos privados
		//////////////////////////////////////////////////////////////////////       
    
		/*Devuelve un nodo con la raiz del arbol*/
		NodoBPlus* getRaiz();
		
		void insertarInterno(NodoBPlus* &nodoDestino, char* codigo, Clave* claveInsertada);		
    
	    void eliminarInterno(NodoBPlus* nodoTarget, char* codigo, Clave* claveEliminada);  
	    
	    NodoBPlus* buscarPadre(NodoBPlus* padre, NodoBPlus* hijo, Clave* claveNodoHijo) const;
	    
	    NodoBPlus* buscarLugar(Clave* clave) const;
	    
	    NodoBPlus* buscarLugarRecursivo(NodoBPlus* nodo, Clave* clave) const;
	    
	    Clave* split(NodoBPlus* nodoTarget);
	    
	    //Mergea nodoConUnderflow con nodoHno; y con el separador si se trata de un nodo interno. No se encarga
	    //de modificar nada en el nodo padre, hay que hacerlo desde afuera.
	    void merge(NodoBPlus* nodoIzq, NodoBPlus* &nodoDer, Clave* separador);
	    
	    void pasarClaveHaciaIzquierda(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre,
	    							  NodoBPlus* nodoHnoDer, Clave* clavePadre);
	    
	    void pasarClaveHaciaDerecha(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre,
	    							NodoBPlus* nodoHnoIzq, Clave* clavePadre);
	    
	  	
}; //Fin clase BPlusTree.

#endif /*BPLUSTREE_H_*/
