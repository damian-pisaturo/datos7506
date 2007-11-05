#include "IndiceHash.h"

IndiceHash::IndiceHash(list<nodoLista> listaParam, unsigned int tamBucket, const string& nombreArchivo) 
:Indice(){

	IndiceHashManager* indiceManager = (IndiceHashManager*) IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
	
	hash = new Hash(indiceManager, listaParam, (char*)indiceManager->getNombreArchivo().c_str());
}

IndiceHash::~IndiceHash() {
	if (hash) delete hash;
}

int IndiceHash::insertarRegistro(char *registro, void **clave) {
	return hash->insertarRegistro(registro, clave);
}
		
int IndiceHash::eliminarRegistro(void **clave) {
	return hash->eliminarRegistro(clave);
}


