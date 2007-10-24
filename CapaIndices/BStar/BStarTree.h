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
		void insertarInterno(Clave* clave);
		
		void eliminarInterno(Clave* clave);
		
		bool puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre, NodoBStar* &nodoHnoDer);
		//Pasa una clave de un hermano derecho a otro nodo
		void pasarClaveDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer);
		
		//Pasa una clave de un hermano izquierdo a otro nodo
		void pasarClaveIzquierda();
		
		//Busca el padre del nodo pasado por parámetro
		NodoBStar* buscarPadre(NodoBStar* nodo) const;
		
		//Busca el nodo en el que se debe insertar una clave
		NodoBStar* buscarLugar(Clave* clave) const;
		
};

#endif /*BSTARTREE_H_*/
