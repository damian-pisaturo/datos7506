#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(const unsigned char tipoIndice, unsigned short tamBloqueLista,
						 int tipoDato, ListaTipos* listaTipos, const unsigned char tipoEstructura,
						 unsigned short tamNodo, unsigned short tamBloqueDato,
						 const string& nombreArchivo, unsigned char tipoOrg) {
	
	this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(tipoIndice, tipoDato, listaTipos, 
																			   tipoEstructura, tamNodo, 0, nombreArchivo);
	this->tipoIndice = tipoIndice;
	this->tamBloqueDato = tamBloqueDato;
	
	switch (tipoEstructura) {
		case TipoIndices::ARBOL_BP:
			this->bTree = new BPlusTree(*indiceManager, tamNodo);
			break;
		case TipoIndices::ARBOL_BS:
			this->bTree = new BStarTree(*indiceManager, tamNodo);
			break;
		default:
			this->bTree = NULL;
	}

	switch (tipoIndice) {
		case TipoIndices::GRIEGO:			
			this->bloqueManager = new BloqueDatosManager(tamBloqueDato, nombreArchivo, tipoOrg);
			break;
		case TipoIndices::ROMANO:
			this->bloqueManager = new BloqueListaManager(tamBloqueLista, nombreArchivo);
			break;
		default:
			this->bloqueManager = NULL;
	}

}

IndiceArbol::~IndiceArbol() {
	if (this->bTree) delete this->bTree;
	if (this->bloqueManager) delete this->bloqueManager;
}

/*
 * Este metodo inserta una clave en un indice.
 */
int IndiceArbol::insertar(Clave *clave, char* &registro) {
	//int resultado = this->bloqueManager->escribirBloqueDatos(registro);
	//if (resultado >= 0) {
	//	clave->setReferencia(resultado);
	//	if (bTree->insertar(clave))
	//		return ResultadosIndices::OK;
	//	else return ResultadosIndices::CLAVE_DUPLICADA;
	//}
	return ResultadosIndices::ERROR_INSERCION;
}

/*
 * Este metodo elimina una clave del indice. 
 * Si la encuentra devuelve OK; y si no, devuelve ERROR_ELIMINACION.
 **/
int IndiceArbol::eliminar(Clave *clave) {
	if (bTree->eliminar(clave)) return ResultadosIndices::OK;
	else return ResultadosIndices::REGISTRO_NO_ENCONTRADO;
}

/*
 * Este metodo busca una clave dentro del indice, y devuelve el
 * bloque de datos correspondiente a la referencia de esa clave.
 **/
int IndiceArbol::buscar(Clave *clave, char* &registro) const {
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	registro = new char[this->getTamanioBloqueDato()];
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), registro);
	if (resultado != ResultadosFisica::OK) {
		delete registro;
		registro = NULL;
		return ResultadosIndices::ERROR_CONSULTA;
	}
	return ResultadosIndices::OK;
}

int IndiceArbol::buscar(Clave *clave) const {
	if (bTree->buscar(clave)) return ResultadosIndices::CLAVE_ENCONTRADA;
	else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
}

/*
 * Devuelve ResultadosIndices::ERROR_MODIFICACION si claveVieja no se encuentra en el indice. 
 * En caso contrario devuelve ResultadosIndices::OK, reemplaza claveVieja
 * por claveNueva y reemplaza el viejo registro correspondiente
 * a "claveVieja" por "registroNuevo".
 **/
int IndiceArbol::modificar(Clave* claveVieja, Clave* claveNueva, char* &registroNuevo) {
	int resultado = bTree->modificar(claveVieja, claveNueva);
	if (resultado == ResultadosIndices::OK) {
		//resultado = this->bloqueManager->escribirBloqueDatos(claveNueva->getReferencia(), registroNuevo);
		//if (resultado == ResultadosFisica::OK) return ResultadosIndices::OK;
		//else return ResultadosIndices::ERROR_MODIFICACION;
	}
	return resultado;
}


int IndiceArbol::buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque) {
	bloqueDatos = new char[this->getTamanioBloqueDato()]; //La memoria de debe liberar afuera
	int resultado = this->bloqueManager->buscarEspacioLibre(bloqueDatos, tamRegistro);
	if (resultado != ResultadosFisica::BLOQUES_OCUPADOS) {
		nroBloque = resultado;
		return ResultadosIndices::REQUIERE_BLOQUE;
	}
	return resultado;
}


