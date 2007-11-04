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
	
}

IndiceArbol::~IndiceArbol() {
	if (bTree) delete bTree;
}

void IndiceArbol::insertar(Clave* clave) {
	bTree->insertar(clave);
}

bool IndiceArbol::eliminar(Clave* clave) {
	return bTree->eliminar(clave);
}

Clave* IndiceArbol::buscar(Clave* clave) const {
	return bTree->buscar(clave);
}

bool IndiceArbol::modificar(Clave* claveVieja, Clave* claveNueva) {
	return bTree->modificar(claveVieja, claveNueva);
}
