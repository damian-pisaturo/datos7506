#ifndef BSTARTREE_H_
#define BSTARTREE_H_

#include "NodoBStar.h"
#include "../Manager/IndiceManager.h"


class BStarTree {
	
	public:
		//Constructor que recibe el tamaño de los nodos (en bytes)
		BStarTree(IndiceManager& indiceManager, unsigned short tamanioNodo);
		
		virtual ~BStarTree();
		
		void insertar(Clave* clave);
		
		//Elimina la Clave clave si la encuentra, y devuelve true. En caso contrario devuelve false.
		bool eliminar(Clave* clave);
		
		Clave* buscar(Clave* clave) const;
		
		//Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario, la reemplaza por claveNueva
		//y devuelve true.
		bool modificar(Clave* claveVieja, Clave* claveNueva);
		
		bool vacio() const { return (this->nodoRaiz == NULL); }
		
		
	private:
		//Atributos
		
		NodoBStar* nodoRaiz;
		//Para administrar la raíz, se utiliza el modelo en el cual la misma
		//posee el espacio suficiente para que a la hora de dividirse,
		//genere dos nodos 2/3 llenos.
		unsigned short tamanioRaiz; //Tamanio máximo (en bytes) de la raíz
		unsigned short tamanioNodo; //Tamanio máximo (en bytes) de un nodo
		
		//Objeto utilizado para cargar y guardar los nodos desde disco
		IndiceManager& indiceManager; 
		
		//Métodos
		
		void insertarInterno(NodoBStar* &nodoDestino, char* codigo);
		
		void eliminarInterno(NodoBStar* nodoTarget, char* codigo);
		
		//bool puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
		//							  	NodoBStar* &nodoHnoDer) const;
		
		//bool puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
		//								  NodoBStar* &nodoHnoIzq) const;
		
		//Pasa una clave de un hermano derecho a otro nodo
		//Se usa en la eliminación de una clave
		void pasarClaveHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
							   		  NodoBStar* nodoHnoDer, Clave* clavePadre);
		
		//Pasa una clave de un hermano izquierdo a otro nodo
		//Se usa en la eliminación de una clave
		void pasarClaveHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
				   					NodoBStar* nodoHnoIzq, Clave* clavePadre);
		
		void recibirClaveDesdeDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
							  		  NodoBStar* nodoHnoDer, Clave* clavePadre);
		
		void recibirClaveDesdeIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre,
										NodoBStar* nodoHnoIzq, Clave* clavePadre);
		
		//Busca el padre del nodo pasado por parámetro
		NodoBStar* buscarPadre(NodoBStar* padre, NodoBStar* hijo) const;
		
		//Busca el nodo en el que se debe insertar/eliminar una clave.
		//Devuelve un puntero al nodo donde se debe insertar/eliminar la clave.
		NodoBStar* buscarLugar(Clave* clave) const;
		
		NodoBStar* buscarClave(NodoBStar* nodo, Clave* clave) const;
		
		NodoBStar* buscarLugarRecursivo(NodoBStar* nodo, Clave* clave) const;
		
		//Se supone que el nodo que recibe no es hoja. Este nodo debe ser el nodo
		//que contiene a la clave que recibe como segundo parámetro.
		//Retorna un nodo cuya primera clave es la menor de las claves mayores a
		//la clave recibida por parámetro.
		NodoBStar* buscarMenorMayores(NodoBStar* nodo, Clave* clave) const;
		
		//Parte dos nodos completos en tres y devuelve las dos claves promocionadas.
		VectorClaves* mergeSplitOverflow(NodoBStar* nodoTarget, NodoBStar* nodoHnoDer, Clave* clavePadre);
		
		//Este método concatena un nodo con underflow con un nodo hermano y con una clave del nodo
		//padre. Esto sucede cuando el árbol sólo tiene dos niveles y el nodo raíz sólo tiene una clave.
		void merge(NodoBStar* &nodoHijoIzq, NodoBStar* &nodoHijoDer, NodoBStar* nodoPadre);
		
		//Este método concatena un nodo con underflow con dos nodos hermanos y con dos claves del nodo
		//padre. Realiza el split internamente y devuelve la clave a promocionar.
		Clave* mergeSplitUnderflow(NodoBStar* nodoTarget, NodoBStar* nodoHno1, NodoBStar* &nodoHno2,
								   Clave* clavePadre1, Clave* clavePadre2);
		
};

#endif /*BSTARTREE_H_*/
