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
			
		
		int insertarRegistro(char *registro, void **clave);
		
		int eliminarRegistro(void **clave);
		
		bool recuperarRegistro(void **clave, char* registro);
		
		
};

#endif /*INDICEHASH_H_*/

