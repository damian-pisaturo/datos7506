#include "IndiceHash.h"

	IndiceHash::IndiceHash(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo) 
	{
		this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
		this->tipoIndice    = TipoIndices::GRIEGO;
		this->tamBloque = tamBucket;
		this->hash = new Hash((IndiceHashManager*)indiceManager, listaParam, tamBucket);		
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

