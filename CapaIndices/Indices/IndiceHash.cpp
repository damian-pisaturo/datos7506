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
	 * Inserta una clave secundaria y pone la clave primaria en la lista invertida correspondiente
	 * a la clave secundaria. Dentro del índice, guarda junto con la clave secundaria, el offset al 
	 * bloque donde se encuantra la lista invertida.
	 * */
	int IndiceHash::insertar(Clave *claveSecundaria, Clave* clavePrimaria)
	{
		char* registro = NULL;
		unsigned short tamanioRegistro = 0;
		
		// Comprueba si la clave secundaria ya está presente en el indice.
		// De ser así, recupera en registro dicha clave, y el offset a la 
		// lista invertida.
		bool encontrado = this->hash->recuperarRegistro(*claveSecundaria, registro, tamanioRegistro);
		
		int resultado = ResultadosIndices::OK;
		
		char* clavePrimariaSerializada = this->hash->serializarClave(clavePrimaria->getValorParaHash());
		
		// Crea un bloque de lista.
		Bloque *bloque =  new BloqueListaPrimaria(Tamanios::TAMANIO_BLOQUE_DATO);
		
		if (encontrado)
		{
			// Obtiene el offset a la lista.
			unsigned int referenciaALista = getOffsetToList(registro, tamanioRegistro);
		
			// Lee de disco el bloque con dicha lista.
			char* bloqueLista = new char[Tamanios::TAMANIO_BLOQUE_DATO];
			if (this->bloqueManager->leerBloqueDatos(referenciaALista, bloqueLista) == ResultadosFisica::OK) {
				
				// Le setea los datos que levantó de disco al bloque, y agerga la clave primaria a 
				// la lista.
				bloque->setDatos(bloqueLista);
				bloque->altaRegistro(this->hash->getListaParametros(), clavePrimariaSerializada);			
				this->bloqueManager->escribirBloqueDatos(referenciaALista, bloque->getDatos());
			
			} else {	
			
				// Si no puede leer la lista de disco, devuelve un error de inserción.
				delete[] bloqueLista;
				bloqueLista = NULL;
				resultado = ResultadosIndices::ERROR_INSERCION;
			}
		
		} else {
			
			// Si la clave secundaria no esta presente en el índice, inicializo el bloque,
			// le inserto la clave primaria, y lo guardo en disco.
			// Si puede guardarlo a disco, en resultado obtengo el offset a donde se guarda la lista.
			bloque->clear();
			bloque->altaRegistro(this->hash->getListaParametros(), clavePrimariaSerializada);
			resultado = this->bloqueManager->escribirBloqueDatos(bloque->getDatos());
			
			if (resultado >= 0) {	
				// Crea un registro donde se guarda la clave secundaria serializada, concatenada con
				// el offset a la lista invertida.
				char* claveSecundariaSerializada = this->hash->serializarClave(claveSecundaria->getValorParaHash());
				registro = new char[strlen(claveSecundariaSerializada) + sizeof(resultado)];
				setOffsetToList(resultado, registro, tamanioRegistro);
				memcpy(registro, claveSecundariaSerializada, strlen(claveSecundariaSerializada));
				
				// Inserta el registro en el indice secundario.
				if (this->hash->insertarRegistro(registro, *claveSecundaria))
					resultado = ResultadosIndices::OK;
				else
					resultado = ResultadosIndices::ERROR_INSERCION;
				
				delete[] claveSecundariaSerializada;
			} else resultado = ResultadosIndices::ERROR_INSERCION;
		
		}
		
		delete[] clavePrimariaSerializada;
		delete[] registro;
		delete bloque;
		
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
	

