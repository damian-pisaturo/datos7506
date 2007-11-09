#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(unsigned char tipoIndice, unsigned short tamBloqueLista,
						 int tipoDato, ListaTipos* listaTipos,
						 unsigned char tipoEstructura, unsigned short tamNodo,
						 const string& nombreArchivo) {
	
	IndiceManager* indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(tipoIndice, tamBloqueLista, tipoDato, listaTipos, 
																					   tipoEstructura, tamNodo, 0, nombreArchivo);
	
	switch (tipoEstructura){
		case TipoIndices::ARBOL_BP:
			bTree = new BPlusTree(*indiceManager, tamNodo);
			break;
		case TipoIndices::ARBOL_BS:
			bTree = new BStarTree(*indiceManager, tamNodo);
			break;
		default:
			bTree = NULL;
	}
	
	this->tipoIndice = tipoIndice;
	
}

IndiceArbol::~IndiceArbol() {
	if (bTree) delete bTree;
}

/*
 * Este metodo inserta una clave en un indice.
 */
int IndiceArbol::insertar(Clave *clave, char* registro) {
	bTree->insertar(clave);
	return OK;
}

/*
 * Este metodo elimina una clave del indice. 
 * Si la encuentra devuelve OK; y si no, devuelve NO_ENCONTRADO.
 **/
int IndiceArbol::eliminar(Clave *clave) {
	return (int)!bTree->eliminar(clave);
}

/*
 * Este metodo busca una clave dentro del indice, y la devuelve con todos
 * sus atributos actualizados.
 **/
Clave* IndiceArbol::buscar(Clave *clave, char* registro) const {
	return bTree->buscar(clave);
}

/*
 * Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario,
 * la reemplaza por claveNueva y devuelve true.
 **/
bool IndiceArbol::modificar(Clave* claveVieja, Clave* claveNueva, char* registroNuevo) {
	return bTree->modificar(claveVieja, claveNueva);
}


char IndiceArbol::buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque) {
	
	//TODO Llamar al método de Nico...
	return 0;
}


Clave* IndiceArbol::buscar(Clave* clave, SetClaves* &setClavesPrimarias) const {
	
	//TODO Lamar al método de Nico...
	return 0;
}
