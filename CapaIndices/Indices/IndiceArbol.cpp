#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(const unsigned char tipoIndice, int tipoDato,
						 ListaNodos* listaTipos, const unsigned char tipoEstructura,
						 unsigned short tamNodo, unsigned short tamBloque,
						 const string& nombreArchivo, unsigned char tipoOrg) {
	
	this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(tipoIndice, tipoDato,
																			   this->getListaTipos(listaTipos), 
																			   tipoEstructura, tamNodo, 0,
																			   nombreArchivo);
	this->tipoIndice = tipoIndice;
	this->tamBloque = tamBloque;
	this->listaNodos = listaTipos;
	
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
			this->bloque = new Bloque(0, tamBloque);
			this->bloqueManager = new BloqueDatosManager(tamBloque, nombreArchivo, tipoOrg);
			break;
		case TipoIndices::ROMANO:
			this->bloque = new BloqueListaPrimaria(tamBloque);
			this->bloqueManager = new BloqueListaManager(tamBloque, nombreArchivo);
			break;
		default:
			this->bloque = NULL;
			this->bloqueManager = NULL;
	}

}

IndiceArbol::~IndiceArbol() {
	if (this->bTree) delete this->bTree;
	if (this->bloque) delete this->bloque;
	if (this->bloqueManager) delete this->bloqueManager;
}

/*
 * Este metodo inserta una clave en un indice.
 */
int IndiceArbol::insertar(Clave *clave, char* &registro, unsigned short tamRegistro) {
	int resultado         = 0;
	int nroBloque         = 0;
	Bloque* bloque        = NULL;
	char* contenidoBloque = NULL;
	Clave* claveBuscada   = bTree->buscar(clave);
	
	if (claveBuscada) 
		resultado =  ResultadosIndices::CLAVE_DUPLICADA;
	else{
		contenidoBloque = new char[this->tamBloque];
		memset(contenidoBloque, 0, this->tamBloque);
		
		// Lee de disco el contenido del bloque donde debe insertar.
		nroBloque = this->buscarBloqueDestino(tamRegistro, contenidoBloque);		
		
		// Si hay lugar en el bloque de disco, le asigno su contenido	
		if ( (nroBloque != ResultadosFisica::BLOQUES_OCUPADOS) && (nroBloque != ResultadosFisica::ARCHIVO_VACIO) ){
			bloque = new Bloque(this->tamBloque);
			
			bloque->setNroBloque(nroBloque);		
			bloque->setDatos(contenidoBloque);
			
			// Inserta el registro.
			bloque->altaRegistro(this->listaNodos,registro);
			
			// Sobreescribe el archivo de datos en la posicion 'nroBloque'
			resultado = this->bloqueManager->escribirBloqueDatos(nroBloque, bloque->getDatos());	
		}else{			
			bloque = new Bloque(0, this->tamBloque);		

			// Inserta el registro.
			bloque->altaRegistro(this->listaNodos, registro);		
			
			// Agrega un nuevo bloque de datos al archivo
			nroBloque = this->bloqueManager->escribirBloqueDatos(bloque->getDatos());
			
			delete[] contenidoBloque;
		}
		
		clave->setReferencia(nroBloque);
		if (bTree->insertar(clave))
			resultado = ResultadosIndices::OK;
	}
	
	if (claveBuscada)
		delete claveBuscada;
	
	if (bloque)
		delete bloque;
	
	return resultado;
}


/*
 * Este metodo inserta una clave en un indice secundario, y su correspondiente
 * lista de claves primarias.
 */
int IndiceArbol::insertar(Clave *clave, char* &registro) {
	Clave* claveBuscada = bTree->buscar(clave);
	
	if (claveBuscada) {
		delete claveBuscada;
		return ResultadosIndices::CLAVE_DUPLICADA;
	}
	
	int resultado = 0;
	//TODO Cuando Manu termine de probar el arbol hay que volver a asignar resultado al escribirBloqueDatos()
	if (registro)
		this->bloqueManager->escribirBloqueDatos(registro);
	
	if (resultado >= 0) {
		clave->setReferencia(resultado);
		if (bTree->insertar(clave))
			resultado = ResultadosIndices::OK;
	}
	
	return resultado;
}


/*
 * Este metodo elimina una clave del indice. 
 * Si la encuentra devuelve OK; y si no, devuelve ERROR_ELIMINACION.
 **/
int IndiceArbol::eliminar(Clave *clave) {
	int resultado = ResultadosIndices::OK;
	Clave* claveBuscada = bTree->buscar(clave);
	if (!claveBuscada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	if (bTree->eliminar(clave)) {
		char *contenidoBloque =  new char[this->tamBloque];
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), contenidoBloque) == ResultadosFisica::OK){
			Bloque* bloque = new Bloque(0, this->tamBloque);
			bloque->setDatos(contenidoBloque);
			bloque->bajaRegistro(this->listaNodos,*claveBuscada);
			resultado = this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), bloque->getDatos());
			delete bloque;
		} else
			delete[] contenidoBloque;
	}
	else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
	delete claveBuscada;
	return resultado;
}

/*
 * Este metodo busca una clave dentro del indice, y devuelve el
 * registro de datos correspondiente a dicha clave.
 **/
int IndiceArbol::buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const {
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	char* bloqueDatos = new char[this->getTamanioBloque()];
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), bloqueDatos);
	
	if (resultado == ResultadosFisica::OK) {
		
		Bloque* bloque = new Bloque(0, this->tamBloque);
		bloque->setDatos(bloqueDatos);
	
		unsigned short offsetToReg = 0;
		
		if (bloque->buscarRegistro(this->listaNodos, *clave, &offsetToReg)){
		
			tamanioRegistro = bloque->getTamanioRegistroConPrefijo(this->listaNodos, bloque->getDatos() + offsetToReg );
			
			if (registro)
				delete[] registro;
			
			registro = new char[tamanioRegistro];
			
			memcpy(registro, bloque->getDatos() + offsetToReg, tamanioRegistro);
			
			resultado = ResultadosIndices::OK;			
			
		} else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
		
		delete bloque;
		
	} else {
		delete[] bloqueDatos;
		registro = NULL;
	}
	
	delete claveRecuperada;
	
	return resultado;
}


int IndiceArbol::buscar(Clave *clave, char* &registro) const {
	
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	if (registro) delete[] registro;
	registro = new char[this->tamBloque];
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), registro);
	
	delete claveRecuperada;
	
	if (resultado != ResultadosFisica::OK) {
		delete registro;
		registro = NULL;
	}
	
	return resultado;	
	
}


int IndiceArbol::buscar(Clave *clave) const {
	Clave* claveBuscada = bTree->buscar(clave);
	if (claveBuscada) {
		delete claveBuscada;
		return ResultadosIndices::CLAVE_ENCONTRADA;
	}
	else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
}

/*
 * Devuelve ResultadosIndices::ERROR_MODIFICACION si claveVieja no se encuentra en el indice. 
 * En caso contrario devuelve ResultadosIndices::OK, reemplaza claveVieja
 * por claveNueva y reemplaza el viejo registro correspondiente
 * a "claveVieja" por "registroNuevo".
 **/
int IndiceArbol::modificar(Clave* claveVieja, Clave* claveNueva, char* &registroNuevo, unsigned short tamanioRegistroNuevo) {
	int resultado = this->eliminar(claveVieja);
	
	if (resultado == ResultadosIndices::OK)
		resultado = this->insertar(claveNueva, registroNuevo, tamanioRegistroNuevo);
	
	return resultado;
}


int IndiceArbol::buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos) {
	return this->bloqueManager->buscarEspacioLibre(bloqueDatos, tamRegistro);
}


ListaTipos* IndiceArbol::getListaTipos(ListaNodos *listaNodos) const {
	
	ListaTipos* listaTipos = new ListaTipos();
	
	for (ListaNodos::iterator it = listaNodos->begin(); it != listaNodos->end(); ++it)
		listaTipos->push_back(it->tipo);
	
	return listaTipos;
	
}
