#ifndef INDICEARBOL_H_
#define INDICEARBOL_H_

#include "../BTree/BTree.h"
#include "../BStar/BStarTree.h"
#include "../BPlus/BPlusTree.h"
#include "../Common/IndiceManagerFactory.h"
#include "Indice.h"

class IndiceArbol: public Indice {
	
	private:
		
		BTree* bTree;
	
	public:
		
		IndiceArbol(unsigned char tipoIndice, unsigned short tamBloqueLista,
					int tipoDato, ListaTipos* listaTipos,
					unsigned char tipoEstructura, unsigned short tamNodo,
					const string& nombreArchivo);
		
		virtual ~IndiceArbol();
		
		void insertar(Clave* clave);
		
		//Elimina la Clave clave si la encuentra, y devuelve true. En caso contrario devuelve false.
		bool eliminar(Clave* clave);
		
		Clave* buscar(Clave* clave) const;
		
		//Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario,
		//la reemplaza por claveNueva y devuelve true.
		bool modificar(Clave* claveVieja, Clave* claveNueva);
		
};

#endif /*INDICEARBOL_H_*/
