#ifndef BTREE_H_
#define BTREE_H_

#include "../Common/Nodo.h"
#include "../Manager/IndiceManager/IndiceManager.h"
#include "../../Common/ResultadosIndices.h"

class BTree {
	
	protected:
		
		unsigned short tamanioNodo; //Tamanio máximo (en bytes) de un nodo
		
		//Objeto utilizado para cargar y guardar los nodos desde disco
		IndiceArbolManager& indiceManager;
		
		
	
	public:
		
		BTree(IndiceManager& indiceManager, unsigned short tamanioNodo)
		: tamanioNodo(tamanioNodo), indiceManager((IndiceArbolManager&)indiceManager) {}
		
		virtual ~BTree() {}
		
		virtual bool insertar(Clave* clave) = 0;
		
		//Elimina la Clave clave si la encuentra, y devuelve true. En caso contrario devuelve false.
		virtual bool eliminar(Clave* clave) = 0;
		
		virtual Clave* buscar(Clave* clave) const = 0;
		
		//Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario,
		//la reemplaza por claveNueva y devuelve true.
		virtual int modificar(Clave* claveVieja, Clave* claveNueva) = 0;
		
		virtual bool vacio() const = 0;
		
		virtual void primero() = 0;
		virtual void mayorOIgual(Clave* clave) = 0;
		virtual void mayor(Clave* clave) = 0;
		
		virtual Clave* siguiente() = 0;
};

#endif /*BTREE_H_*/
