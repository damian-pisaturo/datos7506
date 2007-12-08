#include "IndiceArbol.h"

IndiceArbol::IndiceArbol(unsigned char tipoIndice, int tipoDato,
						 ListaTipos* listaTiposClave, ListaInfoRegistro* listaTipos,
						 unsigned char tipoEstructura, unsigned short tamBloqueDatos,
						 unsigned short tamNodo, unsigned short tamBloqueLista,
						 const string& nombreArchivo, unsigned char tipoOrg) {
	
	this->tipoIndice = tipoIndice;
	this->tipoEstructura = tipoEstructura;
	this->tamBloqueDatos = tamBloqueDatos;
	this->tamBloqueLista = tamBloqueLista;
	this->tipoOrg = tipoOrg;
	this->listaInfoReg = listaTipos;
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
			this->bloque = new Bloque(0, tamBloqueDatos, tipoOrg);
			this->bloqueManager = new BloqueDatosManager(tamBloqueDatos, nombreArchivo, tipoOrg);
			break;
		case TipoIndices::ROMANO:
			this->bloque = new BloqueListaPrimaria(tamBloqueLista);
			this->bloqueManager = new BloqueListaManager(tamBloqueLista, nombreArchivo);
			break;
		default:
			this->bloque = NULL;
			this->bloqueManager = NULL;
	}

}

IndiceArbol::~IndiceArbol() {
	if (this->bTree) delete this->bTree;
	if (this->bloque) delete this->bloque;
	// BloqueManager e IndiceManager se liberan en el destructor de indice.
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
		
		if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
			contenidoBloque = new char[this->getTamanioBloqueDatos()];
			delete contenidoBloque;
	cout << "Saliendo... (hay un exit(0) en la linea 69 de IndiceArbol.cpp" << endl;
	exit(0);
			memset(contenidoBloque, 0, this->getTamanioBloqueDatos());
			// Lee de disco el contenido del bloque donde debe insertar.
			nroBloque = this->buscarBloqueDestino(tamRegistro, contenidoBloque);
			
			// Si hay lugar en el bloque de disco, le asigno su contenido	
			if ( (nroBloque != ResultadosFisica::BLOQUES_OCUPADOS) && (nroBloque != ResultadosFisica::ARCHIVO_VACIO) ){
				this->bloque->setNroBloque(nroBloque);		
				this->bloque->setDatos(contenidoBloque);
				// Inserta el registro.
				this->bloque->altaRegistro(this->listaInfoReg, registro);
				// Sobreescribe el archivo de datos en la posicion 'nroBloque'
				resultado = this->bloqueManager->escribirBloqueDatos(nroBloque, this->bloque->getDatos());
			}else{
				
				// Inserta el registro.
				this->bloque->altaRegistro(this->listaInfoReg, registro);
				
				// Agrega un nuevo bloque de datos al archivo
				nroBloque = this->bloqueManager->escribirBloqueDatos(this->bloque->getDatos());
				
				delete[] contenidoBloque;
			}
			
		} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS)
			// Agrega un nuevo registro de datos al archivo
			nroBloque = this->bloqueManager->escribirBloqueDatos(registro);
		
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
	
	ListaInfoRegistro* listaInfoReg = this->getListaInfoRegClavePrimaria();
	ListaTipos* listaTipos = this->getListaTiposClavePrimaria();
	char* claveSerializada = Bloque::serializarClave(clavePrimaria, listaTipos);
	
	if (claveBuscada) {
		
		char* bloqueLista = new char[this->getTamanioBloqueLista()];
		
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), bloqueLista) == ResultadosFisica::OK) {
			this->bloque->setDatos(bloqueLista);
			this->bloque->altaRegistro(listaInfoReg, claveSerializada);			
			this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
		} else {
			delete[] bloqueLista;
			resultado = ResultadosIndices::ERROR_INSERCION;
		}
		
		delete claveBuscada;
		
	} else {
	
		this->bloque->clear();
		this->bloque->altaRegistro(listaInfoReg, claveSerializada);
		resultado = this->bloqueManager->escribirBloqueDatos(this->bloque->getDatos());
		
		if (resultado >= 0) {			
			claveSecundaria->setReferencia(resultado);
			if (bTree->insertar(claveSecundaria->copiar()))
				resultado = ResultadosIndices::OK;
			else
				resultado = ResultadosIndices::ERROR_INSERCION;
		} else resultado = ResultadosIndices::ERROR_INSERCION;
	
	}
	
	delete listaInfoReg;
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
		
		if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
			
			char *contenidoBloque =  new char[this->getTamanioBloqueDatos()];
			if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), contenidoBloque) == ResultadosFisica::OK){
				this->bloque->setDatos(contenidoBloque);
				this->bloque->bajaRegistro(this->listaInfoReg, *claveBuscada);
				this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
			} else
				delete[] contenidoBloque;
			
		} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS)
			this->bloqueManager->eliminarBloqueDatos(claveBuscada->getReferencia());
		
	} else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
	
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
	
	ListaInfoRegistro* listaInfoReg = this->getListaInfoRegClavePrimaria();
	
	if (claveBuscada) {

		char* bloqueLista = new char[this->getTamanioBloqueLista()];
		
		if (this->bloqueManager->leerBloqueDatos(claveBuscada->getReferencia(), bloqueLista) == ResultadosFisica::OK) {
			this->bloque->setDatos(bloqueLista);
			this->bloque->bajaRegistro(listaInfoReg, *clavePrimaria);			
			if (this->bloque->getCantidadRegistros() == 0) {
				this->bTree->eliminar(claveSecundaria);
				this->bloqueManager->eliminarBloqueDatos(claveBuscada->getReferencia());
			} else 
				this->bloqueManager->escribirBloqueDatos(claveBuscada->getReferencia(), this->bloque->getDatos());
		} else {
			delete[] bloqueLista;
			resultado = ResultadosIndices::ERROR_INSERCION;
		}
		
		delete claveBuscada;
		
	} else resultado = ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
	delete listaInfoReg;
	
	return resultado;
	
}

/*
 * Este metodo busca una clave dentro del indice primario, y devuelve el
 * registro de datos correspondiente a dicha clave.
 **/
int IndiceArbol::buscar(Clave *clave, char* &registro, unsigned short &tamanioRegistro) const {
	tamanioRegistro = 0;
	Clave* claveRecuperada = bTree->buscar(clave);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	char* bloqueDatos = new char[this->getTamanioBloqueDatos()];
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), bloqueDatos);
	
	if (resultado == ResultadosFisica::OK) {
		
		if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
		
			this->bloque->setDatos(bloqueDatos);
		
			unsigned short offsetToReg = 0;
			
			if (this->bloque->buscarRegistro(this->listaInfoReg, *clave, &offsetToReg)) {
			
				tamanioRegistro = this->bloque->getTamanioRegistroConPrefijo(this->listaInfoReg, this->bloque->getDatos() + offsetToReg);
				
				if (registro) delete[] registro;
				
				registro = new char[tamanioRegistro];
				
				memcpy(registro, this->bloque->getDatos() + offsetToReg, tamanioRegistro);
				
				resultado = ResultadosIndices::OK;			
				
			} else resultado = ResultadosIndices::REGISTRO_NO_ENCONTRADO;
			
		} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS) {
			tamanioRegistro = this->getTamanioBloqueDatos();
			registro = bloqueDatos; //La memoria del registro se debe liberar afuera
		}
		
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
int IndiceArbol::buscar(Clave *claveSecundaria, SetClaves* &setClavesPrimarias) const {
	
	Clave* claveRecuperada = bTree->buscar(claveSecundaria);
	if (!claveRecuperada) return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
	char *bloqueLista = new char[this->getTamanioBloqueLista()];
	
	int resultado = this->bloqueManager->leerBloqueDatos(claveRecuperada->getReferencia(), bloqueLista);
	
	if (resultado == ResultadosFisica::OK) {
		
		ListaTipos* listaTiposClavePrimaria = this->getListaTiposClavePrimaria();
		setClavesPrimarias = ((BloqueListaPrimaria*)this->bloque)->getSetClavesPrimarias(bloqueLista, listaTiposClavePrimaria);
		delete listaTiposClavePrimaria;
		
		resultado = ResultadosIndices::OK;
		
	} else resultado = ResultadosIndices::ERROR_CONSULTA;
		
	delete claveRecuperada;
	delete[] bloqueLista;
	
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
	
	int resultado = this->buscar(claveNueva);
	
	if (resultado == ResultadosIndices::CLAVE_NO_ENCONTRADA) {
		
		resultado = this->eliminar(claveVieja);
		if (resultado == ResultadosIndices::OK)
			resultado = this->insertar(claveNueva, registroNuevo, tamanioRegistroNuevo);
		
	} else resultado = ResultadosIndices::CLAVE_DUPLICADA;
	
	return resultado;
}

int IndiceArbol::buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos) {
	return this->bloqueManager->buscarEspacioLibre(bloqueDatos, tamRegistro);
}

SetEnteros IndiceArbol::getConjuntoBloques() {
	SetEnteros conjuntoBloques;
	return conjuntoBloques;
}


Bloque* IndiceArbol::leerBloque(unsigned int nroBloque) {
	char* bloqueDatos = new char[this->getTamanioBloqueDatos()];
	Bloque* bloque = new Bloque(nroBloque, this->getTamanioBloqueDatos(), this->bloque->getTipoOrganizacion());
	this->bloqueManager->leerBloqueDatos(nroBloque, bloqueDatos);
	bloque->setDatos(bloqueDatos);
	return bloque;
}

/*
 * Método que devuelve el siguiente bloque de disco que contiene
 * registros de datos.
 */
int IndiceArbol::siguienteBloque(Bloque* &bloque) {
	
	char* bloqueDatos = new char[this->getTamanioBloqueDatos()];
	int resultado = this->bloqueManager->siguienteBloque(bloqueDatos);
	
	if (resultado == ResultadosFisica::OK) {
		
		if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
			
			bloque = new Bloque(0, this->getTamanioBloqueDatos(), this->getTipoOrganizacion());
			bloque->setDatos(bloqueDatos);
			resultado = ResultadosIndices::OK;
		
		} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS) {
			
			bloque = new Bloque(0, Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + this->getTamanioBloqueDatos(), this->getTipoOrganizacion());
			bloque->altaRegistro(this->getListaInfoReg(), bloqueDatos);
			delete[] bloqueDatos;
			resultado = ResultadosIndices::OK;
			
		}
		
	} else
		delete[] bloqueDatos;
	
	return resultado; //La memoria de 'bloqueDatos' se libera al destruirse 'bloque'
}

void IndiceArbol::primero(){
	this->bTree->primero();
}

void IndiceArbol::mayorOIgual(Clave* clave){
	this->bTree->mayorOIgual(clave);
}

void IndiceArbol::mayor(Clave* clave){
	this->bTree->mayor(clave);
}

Clave* IndiceArbol::siguiente(){
	return this->bTree->siguiente();
}

