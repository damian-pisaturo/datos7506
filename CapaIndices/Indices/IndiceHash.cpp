#include "IndiceHash.h"


	IndiceHash::IndiceHash(unsigned char tipoIndice, ListaNodos *listaParam,
						   unsigned int tamBucket, const string& nombreArchivo) 
	{
		this->indiceManager = IndiceManagerFactory::getInstance().getIndiceManager(TipoIndices::HASH, 0, NULL, TipoIndices::HASH, 0, tamBucket, nombreArchivo);
		this->tipoIndice = tipoIndice;
		this->tamBloque = tamBucket;
		this->hash = new Hash((IndiceHashManager*)indiceManager, listaParam, tamBucket);
		// El tamaño de un bloque de la lista es igual que el tamaño de un bucket
		this->bloqueManager = new BloqueListaManager(tamBucket, nombreArchivo);
		
	}
	
	IndiceHash::~IndiceHash()
	{
		if (this->hash) 
			delete this->hash;
		
		if (this->bloqueManager)
			delete this->bloqueManager;
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
	 * Este método elimina una clave primaria de la lista de claves primarias
	 * correspondiente a una clave secundaria.
	 * Si la lista queda vacía, también se elimina la clave secundaria.
	 */
	int IndiceHash::eliminar(Clave* claveSecundaria, Clave *clavePrimaria)
	{
		char* registro = NULL;
		unsigned short tamanioRegistro = 0;
	
		// Comprueba si la clave secundaria ya está presente en el indice.	
		bool encontrada = this->hash->recuperarRegistro(*claveSecundaria, registro, tamanioRegistro);
		
		int resultado = ResultadosIndices::OK;
	
		Bloque *bloque = new BloqueListaPrimaria(this->tamBloque);
		
		ListaNodos* listaNodos = this->getListaNodosClavePrimaria();
		
		if (encontrada){
			// Si la clave secundaria está en el indice, obtiene su lista de claves primarias.
			
			// Obtiene el offset a la lista.
			unsigned int referenciaALista = getOffsetToList(registro, tamanioRegistro);
			
			char* bloqueLista = new char[this->tamBloque];
			
			// Pone el contenido de la lista en bloqueLista.
			if (this->bloqueManager->leerBloqueDatos(referenciaALista, bloqueLista) == ResultadosFisica::OK) {
				bloque->setDatos(bloqueLista);
				
				// Elimina la clave primaria de la lista, y escribe la lista a disco.
				bloque->bajaRegistro(listaNodos, *clavePrimaria);			
				this->bloqueManager->escribirBloqueDatos(referenciaALista, bloque->getDatos());
				
				// Si ya no quedan claves primarias en la lista, elimina la clave secundaria del indice.
				if (bloque->getCantidadRegistros() == 0)
					this->hash->eliminarRegistro(*claveSecundaria);
			
			} else 
				resultado = ResultadosIndices::ERROR_INSERCION;
			
			delete[] bloqueLista;
		}
		else 
			resultado = ResultadosIndices::CLAVE_NO_ENCONTRADA;	
		
		delete listaNodos;
		delete[] registro;
		delete bloque;
			
		return resultado;
		
	}

	/*
	 * Este metodo busca un registro dentro del indice.
	 * Devuelve el bloque que contiene el registro de clave "clave"
	 * dentro de "bloque".
	 **/
	int IndiceHash::buscar(Clave *clave, char* &bloque, unsigned short &tamanioBloque) const
	{
		tamanioBloque = 0;
		if (this->hash->recuperarRegistro(*clave,bloque,tamanioBloque))
			return ResultadosIndices::OK;
		else return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	}
	
	
	int IndiceHash::buscar(Clave *clave, ListaClaves* &listaClaves) const 
	{
		char *registro = NULL;
		unsigned short tamanioRegistro = 0;
	
		// Comprueba si la clave secundaria está presente en el indice.	
		if (this->hash->recuperarRegistro(*clave, registro, tamanioRegistro))
			return ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
		// Obtiene el offset a la lista.
		unsigned int referenciaALista;
		
		referenciaALista = this->getOffsetToList(registro,tamanioRegistro);
		
		// Se crea un bloque para la lista de claves.
		Bloque *bloque = new BloqueListaPrimaria(this->tamBloque);
		char* bloqueDatos = new char[this->tamBloque];
		
		// Se lee la lista de disco y se carga en bloqueDatos.
		int resultado = this->bloqueManager->leerBloqueDatos(referenciaALista, bloqueDatos);
			
		if (resultado == ResultadosFisica::OK) {
			
			// Se pone en listaClaves una lista de todas las claves primarias que hay en la lista invertida.
			ListaTipos* listaTiposClavePrimaria = this->getListaTiposClavePrimaria();
			listaClaves = ((BloqueListaPrimaria*)bloque)->getListaClaves(bloqueDatos, listaTiposClavePrimaria);
			delete listaTiposClavePrimaria;
			
			resultado = ResultadosIndices::OK;
			
		} else resultado = ResultadosIndices::ERROR_CONSULTA;
			
		delete[] bloqueDatos;
		delete bloque;
		delete[] registro;
		
		return resultado;
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
		ListaNodos* listaNodos = this->getListaNodosClavePrimaria();
		
		// Comprueba si la clave secundaria ya está presente en el indice.
		// De ser así, recupera en registro dicha clave, y el offset a la 
		// lista invertida.
		bool encontrado = this->hash->recuperarRegistro(*claveSecundaria, registro, tamanioRegistro);
		
		int resultado = ResultadosIndices::OK;
		
		char* clavePrimariaSerializada = this->hash->serializarClave(clavePrimaria->getValorParaHash());
		
		// Crea un bloque de lista.
		Bloque *bloque =  new BloqueListaPrimaria(this->tamBloque);
		
		if (encontrado)
		{
			// Obtiene el offset a la lista.
			unsigned int referenciaALista = getOffsetToList(registro, tamanioRegistro);
		
			// Lee de disco el bloque con dicha lista.
			char* bloqueLista = new char[this->tamBloque];
			if (this->bloqueManager->leerBloqueDatos(referenciaALista, bloqueLista) == ResultadosFisica::OK) {
				
				// Le setea los datos que levantó de disco al bloque, y agerga la clave primaria a 
				// la lista.
				bloque->setDatos(bloqueLista);
				bloque->altaRegistro(listaNodos, clavePrimariaSerializada);			
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
			bloque->altaRegistro(listaNodos, clavePrimariaSerializada);
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
		
		delete listaNodos;
		delete[] clavePrimariaSerializada;
		delete[] registro;
		delete bloque;
		
		return resultado;
	}
	
	/*
	 * Devuelve el offset a una lista dentro de un registro de indice secundario.
	 * */
	unsigned int IndiceHash::getOffsetToList(char *registro, unsigned short tamanioRegistro)const
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
	 * Método que devuelve el siguiente bloque de disco que contiene
	 * registros de datos.
	 */
	int IndiceHash::siguienteBloque(Bloque* &bloque) {
		unsigned short cantRegistros = 0;
		char* bloqueDatos = new char[this->tamBloque];
		int resultado = ResultadosFisica::OK;
		
		// Se omiten los buckets vacíos referenciados por la tabla de hash
		while ((cantRegistros == 0) && (resultado == ResultadosFisica::OK)) {
			
			resultado = ((IndiceHashManager*)this->indiceManager)->siguienteBloque(bloqueDatos);
			
			if (resultado == ResultadosFisica::OK)
				memcpy(&cantRegistros, bloqueDatos + Tamanios::TAMANIO_ESPACIO_LIBRE, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		}
		
		if (resultado == ResultadosFisica::OK) {
			bloque = new Bucket(0, 0, this->tamBloque, this->hash->getTipoOrganizacion());
			bloque->setDatos(bloqueDatos);
			resultado = ResultadosIndices::OK;
		} else
			delete[] bloqueDatos;
		
		return resultado; //La memoria de 'bloqueDatos' se libera al destruirse 'bloque'
	}

	
/////////////////////////////////////////////////////////////////////////////////////
// Métodos privados
/////////////////////////////////////////////////////////////////////////////////////
	


	ListaNodos* IndiceHash::getListaNodos() const
	{
		return this->hash->getListaParametros();
	}

