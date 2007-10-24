#ifndef BSTARTREE_H_
#define BSTARTREE_H_

#include "NodoBStar.h"

class BStarTree {
	
	public:
		BStarTree();
		
		virtual ~BStarTree();
		
		void insertar(Clave* clave);
		
		void eliminar(Clave* clave);
		
		
	private:
		//Atributos
		
		NodoBStar* nodoRaiz;
		unsigned short maxTamRaiz; //Tamanio máximo (en bytes) de la raíz
		
		
		//Métodos
		
		void insertarInterno(Clave* clave);
		
		void eliminarInterno(Clave* clave);
		
		bool puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
									  NodoBStar* &nodoHnoDer) const;
		
		//Pasa una clave de un hermano derecho a otro nodo
		void pasarClaveDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
							   NodoBStar* nodoHnoDer);
		
		bool puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
											  NodoBStar* &nodoHnoIzq) const;
		
		//Pasa una clave de un hermano izquierdo a otro nodo
		void pasarClaveIzquierda();
		
		//Busca el padre del nodo pasado por parámetro
		NodoBStar* buscarPadre(NodoBStar* padre, NodoBStar* hijo) const;
		
		//Busca el nodo en el que se debe insertar una clave.
		//Devuelve un puntero al nodo donde se debe insertar la clave,
		//o NULL si la clave ya se encuentra en el árbol.
		NodoBStar* buscarLugar(Clave* clave) const;
		
		NodoBStar* buscarLugarRecursivo(NodoBStar* nodo, Clave* clave) const;
		
		NodoBStar* buscarMenorMayores(NodoBStar* nodo) const;
		
		NodoBStar* buscarMayorMenores(NodoBStar* nodo) const;
		
};

#endif /*BSTARTREE_H_*/
