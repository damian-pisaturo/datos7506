#include "IndiceHash.h"

	IndiceHash::IndiceHash(ListaNodos *listaParam, unsigned int tamBucket, const string& nombreArchivo) 
	{
		IndiceHashManager* indiceManager = (IndiceHashManager*) IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
		
		hash = new Hash(indiceManager, listaParam, tamBucket);
		
		this->tipoIndice = TipoIndices::GRIEGO;
	}
	
	IndiceHash::~IndiceHash() 
	{
		if (this->hash) 
			delete this->hash;
	}

	/*
	 * Este metodo inserta un registro en el indice.
	 **/
	int IndiceHash::insertar(Clave *clave, char* &registro) 
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
	 * dentro de "registro".
	 **/
	int IndiceHash::buscar(Clave *clave, char* &registro) const
	{
		if (this->hash->recuperarRegistro(*clave,registro))
			return ResultadosIndices::OK;
		else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	}
	
	int IndiceHash::buscar(Clave *clave) const
	{
		char* registro = NULL;
		
		if (this->hash->recuperarRegistro(*clave, registro))
			return ResultadosIndices::CLAVE_ENCONTRADA;
		else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	}

	/*
	 * Elimina al registro de clave "claveVieja" e inserta al registro "registroNuevo".
	 **/
	int IndiceHash::modificar(Clave *claveVieja, Clave *claveNueva, char* &registroNuevo) 
	{
		return this->hash->modificarRegistro(*claveVieja, *claveNueva, registroNuevo);
	}


	int IndiceHash::buscarBloqueDestino(unsigned short tamRegistro, char* &bloqueDatos, unsigned int &nroBloque) 
	{	
		//TODO llamar a BloqueDatosManager para que me diga el número de bloque en el que debo insertar
		//el nuevo registro
		return 0;
	}

/*	
	Clave* IndiceHash::buscar(Clave* clave, SetClaves* &setClavesPrimarias) const 
	{		
		//TODO Lamar al método de Nico...
		return 0;
	}
*/

