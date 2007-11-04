#ifndef INDICEHASH_H_
#define INDICEHASH_H_

#include "../Hash/Hash.h"
#include "../Manager/IndiceManager.h"
#include "../Common/IndiceManagerFactory.h"

class IndiceHash {
	
	private:
	
		Hash* hash;
		
		
	public:
		IndiceHash(list<nodoLista> listaParam, unsigned int tamBucket, const string& nombreArchivo);
		virtual ~IndiceHash();
			
		
		int insertarRegistro(char *registro, char *clave);
		
		int eliminarRegistro(char *clave);
		
		
};

#endif /*INDICEHASH_H_*/

