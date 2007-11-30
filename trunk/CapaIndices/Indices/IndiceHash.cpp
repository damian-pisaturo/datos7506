#include "IndiceHash.h"


	IndiceHash::IndiceHash(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo) 
	{
		this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
		this->tipoIndice    = TipoIndices::GRIEGO;
		this->tamBloque = tamBucket;
		this->hash = new Hash((IndiceHashManager*)indiceManager, listaParam, tamBucket);	
		this->bloqueManager = new BloqueListaManager(Tamanios::TAMANIO_BLOQUE_DATO, nombreArchivo );
		
	}
	
	IndiceHash::~IndiceHash()
	{
		if (this->hash) 
			delete this->hash;
	}

	/*
	 * Este metodo inserta un registro en el indice.
	 **/
	int IndiceHash::insertar(Clave *clave, char* &registro, unsigned short tamanioRegistro) 
	{
		return this->hash->insertarRegistro(registro, *clave);
	}

	/*
	 * Este metodo elimina un registro del indice.
	 **/
	int IndiceHash::eliminar(Clave *clave)  
	{
		return this->hash->eliminarRegistro(*clave);
	}

	/*
	 * Este metodo busca un registro dentro del indice.
	 * Devuelve el bloque que contiene el registro de clave "clave"
	 * dentro de "bloque".
	 **/
	int IndiceHash::buscar(Clave *clave, char* &bloque, unsigned short &tamanioBloque) const
	{
		if (this->hash->recuperarRegistro(*clave,bloque,tamanioBloque))
			return ResultadosIndices::OK;
		else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	}
	
	int IndiceHash::buscar(Clave *clave) const
	{
		char* registro = NULL;

		unsigned short tam = 0;
		if (this->hash->recuperarRegistro(*clave, registro,tam))
			return ResultadosIndices::CLAVE_ENCONTRADA;
		else
			return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	}

	/*
	 * Elimina al registro de clave "claveVieja" e inserta al registro "registroNuevo".
	 **/
	int IndiceHash::modificar(Clave *claveVieja, Clave *claveNueva, char* &bloque, unsigned short tamanioRegistroNuevo) 
	{
		return this->hash->modificarRegistro(*claveVieja, *claveNueva, bloque);
	}


	//El Hash no necesita esta funcionalidad por lo que retorna 0 para indicar que es un índice de este tipo
	int IndiceHash::buscarBloqueDestino(unsigned short tamRegistro, char* bloqueDatos) 
	{	
		return 0;
	}
	
	
	
	SetEnteros IndiceHash::getConjuntoBloques()
	{
		SetEnteros conjuntoBloques;
		unsigned int tamanioTabla;
		unsigned int *tabla = this->hash->getCopiaTabla(tamanioTabla);
		SetEnteros::iterator it;
		
		for(unsigned int i = 0; i<tamanioTabla; i++)
			conjuntoBloques.insert(tabla[i]);
		
		delete[] tabla;
		return conjuntoBloques;
	}

	
	Bloque* IndiceHash::leerBloque(unsigned int nroBloque)
	{
		return this->hash->leerBucket(nroBloque);
	}

	/*
	 * Inserta la clave primaria de la clave secundaria en la lista de la misma.
	 * */
	int IndiceHash::insertar(Clave *claveSecundaria, Clave* clavePrimaria)
	{
		char* registro = NULL;
		unsigned short tamanioRegistro = 0;
		bool encontrado = this->hash->recuperarRegistro(*claveSecundaria, registro, tamanioRegistro);
		int resultado = ResultadosIndices::OK;
		char* claveSerializada = this->hash->serializarClave(claveSecundaria->getValorParaHash());
		Bloque *bloque =  new BloqueListaPrimaria(Tamanios::TAMANIO_BLOQUE_DATO);
		if (encontrado)
		{
			char* bloqueLista = new char[Tamanios::TAMANIO_BLOQUE_DATO];
			unsigned int referenciaALista = getOffsetToList(registro, tamanioRegistro);
			
			if (this->bloqueManager->leerBloqueDatos(referenciaALista, bloqueLista) == ResultadosFisica::OK) {
				bloque->setDatos(bloqueLista);
				bloque->altaRegistro(this->hash->getListaParametros(), claveSerializada);			
				this->bloqueManager->escribirBloqueDatos(referenciaALista, bloque->getDatos());
			} else {
				delete[] bloqueLista;
				resultado = ResultadosIndices::ERROR_INSERCION;
			}
		} else {
			bloque->clear();
			bloque->altaRegistro(this->hash->getListaParametros(), claveSerializada);
			resultado = this->bloqueManager->escribirBloqueDatos(bloque->getDatos());
			
			if (resultado >= 0) {			
				registro = new char[strlen(claveSerializada) + sizeof(resultado)];
				setOffsetToList(resultado, registro, tamanioRegistro);
				memcpy(registro, claveSerializada, strlen(claveSerializada));
				
				if (this->hash->insertarRegistro( registro, *claveSecundaria))
					resultado = ResultadosIndices::OK;
				else
					resultado = ResultadosIndices::ERROR_INSERCION;
			} else resultado = ResultadosIndices::ERROR_INSERCION;
		
		}
		return resultado;
	}
	
	/*
	 * Devuelve el offset a una lista dentro de un registro de indice secundario.
	 * */
	unsigned int IndiceHash::getOffsetToList(char *registro, unsigned short tamanioRegistro)
	{
		unsigned int offset;
		memcpy(&offset, registro + tamanioRegistro - sizeof(offset), sizeof(offset));
		return offset;
	}
	
	void IndiceHash::setOffsetToList(unsigned int offset, char *registro, unsigned short tamanioRegistro)
	{
		memcpy(registro + tamanioRegistro - sizeof(offset), &offset, sizeof(offset));
	}
	/*
	 * int IndiceArbol::insertar(Clave *claveSecundaria, Clave* clavePrimaria) {
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
	
		this->bloque->clear();
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
	 * 
	 * */

