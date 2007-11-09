#include "IndiceHash.h"

IndiceHash::IndiceHash(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo) {

	IndiceHashManager* indiceManager = (IndiceHashManager*) IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
	
	hash = new Hash(indiceManager, listaParam, nombreArchivo, tamBucket);
	
	this->tipoIndice = TipoIndices::GRIEGO;
}

IndiceHash::~IndiceHash() {
	if (hash) delete hash;
}

/*
 * Este metodo inserta un registro en el indice.
 **/
int IndiceHash::insertar(Clave *clave, char* registro) {
	return hash->insertarRegistro(registro, *clave);
}

/*
 * Este metodo elimina un registro del indice.
 **/
int IndiceHash::eliminar(Clave *clave) {
	return hash->eliminarRegistro(*clave);
}

/*
 * Este metodo busca un registro dentro del indice.
 * Siempre retorna NULL y devuelve el registro de clave "clave"
 * dentro de "registro".
 **/
Clave* IndiceHash::buscar(Clave *clave, char* registro) const {
	hash->recuperarRegistro(*clave,registro);
	return NULL;
}

/*
 * Elimina al registro de clave "claveVieja" e inserta al registro "registroNuevo".
 **/
bool IndiceHash::modificar(Clave *claveVieja, Clave *claveNueva, char* registroNuevo) {
	return hash->modificarRegistro(*claveVieja, *claveNueva, registroNuevo);
}


char IndiceHash::buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque) {
	
	//TODO Llamar al método de Nico
	return 0;
}


Clave* IndiceHash::buscar(Clave* clave, SetClaves* &setClavesPrimarias) const {
	
	//TODO Lamar al método de Nico...
	return 0;
}
