#ifndef BSTARTREE_H_
#define BSTARTREE_H_

#include "NodoBStar.h"

class BStarTree {
	
	public:
		//Constructor que recibe el tamaño de los nodos (en bytes)
		BStarTree(unsigned short tamanioNodo);
		
		virtual ~BStarTree();
		
		void insertar(Clave* clave);
		
		void eliminar(Clave* clave);
		
		Clave* buscar(Clave* clave) const;
		
		void modificar(Clave* clave);
		
		
	private:
		//Atributos
		
		NodoBStar* nodoRaiz;
		//Para administrar la raíz, se utiliza el modelo en el cual la misma
		//posee el espacio suficiente para que a la hora de dividirse,
		//genere dos nodos 2/3 llenos.
		unsigned short tamanioRaiz; //Tamanio máximo (en bytes) de la raíz
		unsigned short tamanioNodo; //Tamanio máximo (en bytes) de un nodo
		
		
		//Métodos
		
		void insertarInterno(NodoBStar* &nodoDestino, char* codigo);
		
		void eliminarInterno(NodoBStar* nodoTarget, Clave* clave, char* codigo);
		
		bool puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
									  NodoBStar* &nodoHnoDer) const;
		
		//Pasa una clave de un hermano derecho a otro nodo
		//Se usa en la eliminación de una clave
		void pasarClaveHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
							   NodoBStar* nodoHnoDer);
		
		void recibirClaveDesdeDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
									  NodoBStar* nodoHnoDer);
		
		bool puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
											  NodoBStar* &nodoHnoIzq) const;
		
		//Pasa una clave de un hermano izquierdo a otro nodo
		//Se usa en la eliminación de una clave
		void pasarClaveHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
				   NodoBStar* nodoHnoIzq);
		
		void recibirClaveDesdeIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
										NodoBStar* nodoHnoIzq);
		
		//Busca el padre del nodo pasado por parámetro
		NodoBStar* buscarPadre(NodoBStar* padre, NodoBStar* hijo) const;
		
		//Busca el nodo en el que se debe insertar/eliminar una clave.
		//Devuelve un puntero al nodo donde se debe insertar/eliminar la clave.
		NodoBStar* buscarLugar(Clave* clave) const;
		
		NodoBStar* buscarLugarRecursivo(NodoBStar* nodo, Clave* clave) const;
		
		//Se supone que el nodo que recibe no es hoja. Este nodo debe ser el nodo
		//que contiene a la clave que recibe como segundo parámetro.
		//Retorna un nodo cuya primera clave es la menor de las claves mayores a
		//la clave recibida por parámetro.
		NodoBStar* buscarMenorMayores(NodoBStar* nodo, Clave* clave) const;
		
		//Parte dos nodos completos en tres y devuelve las dos claves promocionadas.
		VectorClaves* split(NodoBStar* nodoTarget, NodoBStar* nodoHnoDer, Clave* clavePadre);
		
};

#endif /*BSTARTREE_H_*/
