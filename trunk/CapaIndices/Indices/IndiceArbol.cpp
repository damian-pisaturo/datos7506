#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(const unsigned char tipoIndice, int tipoDato,
						 ListaTipos* listaTiposClave,
						 ListaNodos* listaTipos, const unsigned char tipoEstructura,
						 unsigned short tamNodo, unsigned short tamBloque,
						 const string& nombreArchivo, unsigned char tipoOrg) {
	
	this->tipoIndice = tipoIndice;
	this->tamBloque = tamBloque;
	this->listaNodos = listaTipos;
	this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(tipoIndice, tipoDato,
																			   new ListaTipos(*listaTiposClave), 
																			   tipoEstructura, tamNodo, 0,
																			   nombreArchivo);
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
			this->bloque = new Bloque(0, tamBloque, tipoOrg);
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
			this->bloque->setNroBloque(nroBloque);		
			this->bloque->setDatos(contenidoBloque);
			// Inserta el registro.
			this->bloque->altaRegistro(this->listaNodos, registro);
			// Sobreescribe el archivo de datos en la posicion 'nroBloque'
			resultado = this->bloqueManager->escribirBloqueDatos(nroBloque, this->bloque->getDatos());
		}else{
			
			// Inserta el registro.
			this->bloque->altaRegistro(this->listaNodos, registro);
			
			// Agrega un nuevo bloque de datos al archivo
			nroBloque = this->bloqueManager->escribirBloqueDatos(this->bloque->getDatos());
			
			delete[] contenidoBloque;
		}
		
		resultado = nroBloque;
		
		clave->setReferencia(nroBloque);
		if (bTree->insertar(clave->copiar()))
			resultado = ResultadosIndices::OK;
	}
	
	if (claveBuscada)
		delete claveBuscada;
	
	return resultado;
}


/*
 * Este metodo inserta una clave primaria en la lista de claves primarias
 * de una clave secundaria. 
 */
int IndiceArbol::insertar(Clave *claveSecundaria, Clave* clavePrimaria) {
	Clave* claveBuscada = bTree->buscar(claveSecundaria);
	int resultado = ResultadosIndices::OK;
	
	ListaNodos* listaNodos = this->getListaNodosClavePrimaria();
	ListaTipos* listaTipos = this->getListaTiposClavePrimaria();
	char* claveSerializada = Bloque::serializarClave(clavePrimaria, listaTipos);
	
	if (claveBuscada) {

		char* bloqueLista = new char[this->tamBloque];
		
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), bloqueLista) == ResultadosFisica::OK) {
			this->bloque->setDatos(bloqueLista);
			this->bloque->altaRegistro(listaNodos, claveSerializada);
			this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
		} else {
			delete[] bloqueLista;
			resultado = ResultadosIndices::ERROR_INSERCION;
		}
		
		delete claveBuscada;
		
	} else {
	
		this->bloque->altaRegistro(listaNodos, claveSerializada);
		resultado = this->bloqueManager->escribirBloqueDatos(this->bloque->getDatos());
		
		if (resultado >= 0) {
			claveSecundaria->setReferencia(resultado);
			if (bTree->insertar(claveSecundaria->copiar()))
				resultado = ResultadosIndices::OK;
			else
				resultado = ResultadosIndices::ERROR_INSERCION;
		} else resultado = ResultadosIndices::ERROR_INSERCION;
	
	}
	
	delete listaNodos;
	delete listaTipos;
	delete[] claveSerializada;
	
	return resultado;
}


/*
 * Este metodo elimina una clave del indice primario. 
 * Si la encuentra devuelve OK; y si no, devuelve ERROR_ELIMINACION.
 **/
int IndiceArbol::eliminar(Clave *clave) {
	int resultado = ResultadosIndices::OK;
	Clave* claveBuscada = bTree->buscar(clave);
	if (!claveBuscada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	if (bTree->eliminar(clave)) {
		char *contenidoBloque =  new char[this->tamBloque];
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), contenidoBloque) == ResultadosFisica::OK){
			this->bloque->setDatos(contenidoBloque);
			this->bloque->bajaRegistro(this->listaNodos, *claveBuscada);
			this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
		} else
			delete[] contenidoBloque;
	}
	else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
	delete claveBuscada;
	return resultado;
}


/*
 * Este método elimina una clave primaria de la lista de claves primarias
 * correspondiente a una clave secundaria.
 * Si la lista queda vacía, también se elimina la clave secundaria.
 */
int IndiceArbol::eliminar(Clave* claveSecundaria, Clave *clavePrimaria) {
	
	Clave* claveBuscada = bTree->buscar(claveSecundaria);
	int resultado = ResultadosIndices::OK;
	
	ListaNodos* listaNodos = this->getListaNodosClavePrimaria();
	
	if (claveBuscada) {

		char* bloqueLista = new char[this->tamBloque];
		
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), bloqueLista) == ResultadosFisica::OK) {
			this->bloque->setDatos(bloqueLista);
			this->bloque->bajaRegistro(listaNodos, *clavePrimaria);
			if (this->bloque->getCantidadRegistros() == 0) {
				this->bloqueManager->eliminarBloqueDatos(claveBuscada->getReferencia());
				this->bTree->eliminar(claveSecundaria);
			} else
				this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
		} else {
			delete[] bloqueLista;
			resultado = ResultadosIndices::ERROR_INSERCION;
		}
		
		delete claveBuscada;
		
	} else resultado = ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
	delete listaNodos;
	
	return resultado;
	
}

/*
 * Este metodo busca una clave dentro del indice primario, y devuelve el
 * registro de datos correspondiente a dicha clave.
 **/
int IndiceArbol::buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const {
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	char* bloqueDatos = new char[this->getTamanioBloque()];
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), bloqueDatos);
	
	if (resultado == ResultadosFisica::OK) {
		
		this->bloque->setDatos(bloqueDatos);
	
		unsigned short offsetToReg = 0;
		
		if (this->bloque->buscarRegistro(this->listaNodos, *clave, &offsetToReg)) {
		
			tamanioRegistro = this->bloque->getTamanioRegistroConPrefijo(this->listaNodos, this->bloque->getDatos() + offsetToReg);
			
			if (registro) delete[] registro;
			
			registro = new char[tamanioRegistro];
			
			memcpy(registro, this->bloque->getDatos() + offsetToReg, tamanioRegistro);
			
			resultado = ResultadosIndices::OK;			
			
		} else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
		
	} else {
		delete[] bloqueDatos;
		registro = NULL;
	}
	
	delete claveRecuperada;
	
	return resultado;
}

/*
 * Este método busca una clave secundaria y devuelve la lista de claves primarias
 * correspondiente a dicha clave.
 */
int IndiceArbol::buscar(Clave *claveSecundaria, ListaClaves* &listaClavesPrimarias) const {
	
	Clave* claveRecuperada = bTree->buscar(claveSecundaria);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
	char *bloqueDatos = new char[this->tamBloque];
	
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), bloqueDatos);
	
	if (resultado == ResultadosFisica::OK) {
		
		ListaTipos* listaTiposClavePrimaria = this->getListaTiposClavePrimaria();
		listaClavesPrimarias = ((BloqueListaPrimaria*)this->bloque)->getListaClaves(bloqueDatos, listaTiposClavePrimaria);
		delete listaTiposClavePrimaria;
		
		resultado = ResultadosIndices::OK;
		
	} else resultado = ResultadosIndices::ERROR_CONSULTA;
		
	delete claveRecuperada;
	delete bloqueDatos;
	
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
 * Este método se usa para claves primarias.
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


ListaTipos* IndiceArbol::getListaTipos() const {
	
	ListaTipos* listaTipos = new ListaTipos();
	
	for (ListaNodos::iterator it = ++(this->listaNodos->begin()); it != this->listaNodos->end(); ++it)
		listaTipos->push_back(it->tipo);
	
	return listaTipos;
	
}


ListaTipos* IndiceArbol::getListaTiposClavePrimaria() const {
	
	ListaTipos* listaTipos = new ListaTipos();
	ListaNodos::iterator it = this->listaNodos->begin();
	unsigned short cantClaves = it->cantClaves, i = 0;
	
	for (++it; (i < cantClaves) && (it != this->listaNodos->end()); ++it) {
		if (it->pk == "true") {
			listaTipos->push_back(it->tipo);
			++i;
		}
	}
	
	return listaTipos;
	
}


ListaNodos* IndiceArbol::getListaNodosClavePrimaria() const {
	
	ListaNodos* listaTipos = new ListaNodos();
	ListaNodos::iterator it = this->listaNodos->begin();
	unsigned short i = 0;
	
	nodoLista nodo;
	nodo.cantClaves = it->cantClaves;
	nodo.pk = "";
	nodo.tipo = TipoDatos::TIPO_VARIABLE;
	
	listaTipos->push_back(nodo);
	
	for (++it; (i < nodo.cantClaves) && (it != this->listaNodos->end()); ++it) {
		if (it->pk == "true") {
			listaTipos->push_back(*it);
			++i;
		}
	}
	
	return listaTipos;
	
}


SetEnteros IndiceArbol::getConjuntoBloques() {
	SetEnteros conjuntoBloques;
	return conjuntoBloques;
}


Bloque* IndiceArbol::leerBloque(unsigned int nroBloque) {
	char* bloqueDatos = new char[this->tamBloque];
	this->bloqueManager->leerBloqueDatos(nroBloque, bloqueDatos);
	this->bloque->setDatos(bloqueDatos);
	return this->bloque;
}

/*
 * Método que devuelve el siguiente bloque de disco que contiene
 * registros de datos.
 */
int IndiceArbol::siguienteBloque(Bloque* &bloque) {
	char* bloqueDatos = new char[this->tamBloque];
	int resultado = this->bloqueManager->siguienteBloque(bloqueDatos);
	if (resultado != ResultadosFisica::FIN_BLOQUES) {
		bloque = new Bloque(0, this->tamBloque, this->bloque->getTipoOrganizacion());
		bloque->setDatos(bloqueDatos);
		if (resultado == ResultadosFisica::OK) resultado = ResultadosIndices::OK;
	} else {
		delete[] bloqueDatos;
		resultado = ResultadosIndices::FIN_BLOQUES;
	}
	return resultado; //La memoria de 'bloqueDatos' se libera al destruirse 'bloque'
}

