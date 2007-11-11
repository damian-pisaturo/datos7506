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
		
		/*
		 * Este metodo inserta una clave en un indice.
		 **/
		int insertar(Clave *clave, char* &registro);
		
		/*
		 * Este metodo elimina una clave del indice. 
		 * Si la encuentra devuelve OK; y si no, devuelve NO_ENCONTRADO.
		 **/
		int eliminar(Clave *clave);
		
		/*
		 * Este metodo busca una clave dentro del indice, y la devuelve con todos
		 * sus atributos actualizados.
		 **/
		Clave* buscar(Clave *clave, char* &registro) const;
		
		Clave* buscar(Clave *clave) const;
		
		Clave* buscar(Clave* clave, SetClaves* &setClavesPrimarias) const;
		
		/*
		 * Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario,
		 * la reemplaza por claveNueva y devuelve true.
		 **/
		bool modificar(Clave* claveVieja, Clave* claveNueva, char* &registroNuevo);
		
		char buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque);
						                       
		
};

#endif /*INDICEARBOL_H_*/
