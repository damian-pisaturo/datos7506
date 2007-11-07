#ifndef INDICEHASH_H_
#define INDICEHASH_H_

#include "../Hash/Hash.h"
#include "../Manager/IndiceManager.h"
#include "../Common/IndiceManagerFactory.h"
#include "Indice.h"

class IndiceHash :public Indice {
	
	private:
	
		Hash* hash;
		
		
	public:
		IndiceHash(list<nodoLista> &listaParam, unsigned int tamBucket, const string& nombreArchivo);
		virtual ~IndiceHash();	
		
		
		/*
		 * Este metodo inserta un registro en el indice.
		 **/
		int insertar(Clave &clave, char* registro = NULL);
		
		/*
		 * Este metodo elimina un registro del indice.
		 **/
		int eliminar(Clave &clave);
		
		/*
		 * Este metodo busca un registro dentro del indice.
		 * Siempre retorna NULL y devuelve el registro de clave "clave"
		 * dentro de "registro".
		 **/
		Clave* buscar(Clave &clave, char* registro = NULL) const; 
	
		/*
		 * Elimina al registro de clave "claveVieja" e inserta al registro "registroNuevo".
		 **/
		bool modificar(Clave &claveVieja, Clave &claveNueva, char* registroNuevo);
	
};

#endif /*INDICEHASH_H_*/

