#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(const unsigned char tipoIndice, unsigned short tamBloqueLista,
						 int tipoDato, ListaTipos* listaTipos,
						 const unsigned char tipoEstructura, unsigned short tamNodo,
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
int IndiceArbol::insertar(Clave *clave, char* &registro) {
	int resultado;
	//TODO llamar a BloqueDatosManager para guardar el bloque de datos
	//clave->setReferencia(numBloque);
	bTree->insertar(clave);
	return resultado;
}

/*
 * Este metodo elimina una clave del indice. 
 * Si la encuentra devuelve OK; y si no, devuelve ERROR_ELIMINACION.
 **/
int IndiceArbol::eliminar(Clave *clave) {
	if (bTree->eliminar(clave)) return ResultadosIndices::OK;
	else return ResultadosIndices::ERROR_ELIMINACION;
}

/*
 * Este metodo busca una clave dentro del indice, y devuelve el
 * bloque de datos correspondiente a la referencia de esa clave.
 **/
int IndiceArbol::buscar(Clave *clave, char* &registro) const {
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	//TODO llamar a BloqueDatosManager para cargar el bloque de datos
	//correspondiente a la referencia de la clave
	return ResultadosIndices::OK;
}

int IndiceArbol::buscar(Clave *clave) const {
	if (bTree->buscar(clave)) return ResultadosIndices::CLAVE_ENCONTRADA;
	else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
}

/*
 * Devuelve false si claveVieja no se encuentra insertada en el arbol. En caso contrario,
 * la reemplaza por claveNueva y devuelve true.
 **/
int IndiceArbol::modificar(Clave* claveVieja, Clave* claveNueva, char* &registroNuevo) {
	//TODO llamar a BloqueDatosManager para sobreescribir el bloque y obtener el número
	//de bloque para settearselo a la clave nueva
	if (bTree->modificar(claveVieja, claveNueva))
		return ResultadosIndices::OK;
	else return ResultadosIndices::ERROR_MODIFICACION;
}


int IndiceArbol::buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque) {
	
	//TODO llamar a BloqueDatosManager para que me diga el número de bloque en el que debo insertar
	//el nuevo registro
	return 0;
}

/*
Clave* IndiceArbol::buscar(Clave* clave, SetClaves* &setClavesPrimarias) const {
	
	//TODO Lamar al método de Nico...
	return 0;
}
*/

