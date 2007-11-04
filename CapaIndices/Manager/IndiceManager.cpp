///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.cpp
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoIndice correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string) e indice (Griego y Romano).
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "IndiceManager.h"
#include "../Hash/Bucket.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de la capa de indices).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	IndiceManager::IndiceManager(unsigned int tamanioBloque, string nombreArchivo, unsigned char tipoIndice)
	{		
		this->tamanioBloque = tamanioBloque;
		this->tipoIndice    = (unsigned int)tipoIndice;
		
		switch(tipoIndice){
		case TipoIndices::ARBOL_BP:
			this->nombreArchivo = nombreArchivo + ".plus";
			break;
		case TipoIndices::ARBOL_BS:
			this->nombreArchivo = nombreArchivo + ".star";
			break;
		case TipoIndices::HASH:
			this->nombreArchivo = nombreArchivo + ".hash";
			break;
		default:
			this->nombreArchivo = nombreArchivo;
			break;
		}		
	}
	
	IndiceManager::~IndiceManager()
	{ }
	
	ComuDatos* IndiceManager::instanciarPipe(string nombreEjecutable)
	{
		return new ComuDatos(nombreEjecutable);
	}			

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceArbolManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices en arboles de la capa de indices).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor
	///////////////////////////////////////////////////////////////////////
	IndiceArbolManager::IndiceArbolManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
		IndiceManager(tamNodo, nombreArchivo, tipoIndice)
	{ 
		//TODO Usar ComuDato para escribir la raiz vacia si el 
		//archivo de indice esta vacio.
		
		/*
		//Si el archivo de indice esta vacio creo una raiz hoja sin claves
		if(this->archivoIndex->fin())
			escribirRaizVacia();
		*/
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////	
	void IndiceArbolManager::escribirRaizVacia()
	{
		char* buffer = new char[this->getTamanioBloque()];
		char* puntero = buffer;
		HeaderNodo headerNodo;
		
		/*Copiar el header al buffer*/
		headerNodo.nivel   = 0;  
		headerNodo.refNodo = 0;	 
		headerNodo.espacioLibre = this->getTamanioBloque() - this->getTamanioHeader();
		
		memcpy(buffer,&headerNodo,this->getTamanioHeader());
		puntero += this->getTamanioHeader();
		
		/*Escribir la raiz*/
		//TODO Usar ComuDato
		//this->archivoIndex->escribir(buffer);
		
		delete[] buffer;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
	void IndiceArbolManager::copiarClaveHoja(Clave* clave, char* &puntero)
	{		
		//Copio el valor de la clave
		memcpy(puntero, clave->getValor(), clave->getTamanioValor());
		puntero += clave->getTamanioValor();
		
		//Copia de la referencia a registro del archivo de datos. 
		unsigned int referencia = clave->getReferencia();
		memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
	} 
		
	void IndiceArbolManager::copiarClaveNoHoja(Clave* clave, char* &puntero)
	{
		unsigned int referencia = 0;				
		
		//Copia del valor de la clave
		memcpy(puntero, clave->getValor(), clave->getTamanioValor());
		puntero += clave->getTamanioValor();
		
		if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
			//Si el arbol es B*, copiar referencia al registro de datos.			
			referencia = clave->getReferencia();
			memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
			puntero += Tamanios::TAMANIO_REFERENCIA;
		}
		
		//Copia de la referencia al hijo derecho.
		referencia = clave->getHijoDer();
		memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
	}
			
	int IndiceArbolManager::leerBloque(unsigned int numBloque, BloqueIndice* bloqueLeido)
	{
		int resultado = 0;
		Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);
		string buffer;
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;		
		Clave* claveNueva;
		SetClaves* set = new SetClaves();
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica para
		//leer un nodo de disco.
		pipe->agregarParametro(OperacionesCapas::FISICA_LEER_NODO, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		pipe->agregarParametro(numBloque, 3); //Numero de nodo a leer dentro del archivo
	
		//Se lanza el proceso de la capa fisica. 
		//Se obtiene en buffer el contenido del Nodo solicitado.
		pipe->lanzar();
		pipe->leer(this->getTamanioBloque(), buffer);
		
		char * data = (char*) buffer.c_str();

		//Castear el header
		memcpy(&headerNodo, data, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Setear el espacio libre, si es hoja y el HijoIzq
		nodoLeido->setNivel(headerNodo.nivel);
		nodoLeido->setEspacioLibre(headerNodo.espacioLibre); 
		nodoLeido->setRefNodo(headerNodo.refNodo);
		
		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.			
		const char* punteroFinal = data + (this->getTamanioBloque() - headerNodo.espacioLibre);
		
		if(nodoLeido->getNivel() == 0){
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = this->leerClaveHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}	 
		}else{
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = this->leerClaveNoHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}
		}
		
		//Agregar el setClaves al nodo
		nodoLeido->setClaves(set);
		//Setear la posicion del nodo en el archivo
		nodoLeido->setPosicionEnArchivo(numBloque);	
		
		pipe->leer(&resultado);		
		pipe->liberarRecursos();
		
		return resultado;
	}
	
	int IndiceArbolManager::escribirBloque(BloqueIndice* bloqueNuevo)
	{	
		int resultado = 0;
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);
		
		//Variables de escritura del buffer
		string buffer;
		char* data = new char[this->getTamanioBloque()];
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
		//nodoNuevo->actualizarEspacioLibre(this); //NADIE SABIA.
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica para
		//escribir un nodo en disco.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_NODO, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoNuevo->getNivel();  
		headerNodo.refNodo = nodoNuevo->getRefNodo();
		headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
		//punteroAux apunta al inicio del buffer de datos.
		//Necesario para crear un elemento de tipo string.
		char* punteroAux = data;
		memcpy(data, &headerNodo, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Obtener la lista de claves
		set = nodoNuevo->getClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
		if(nodoNuevo->getNivel()==0){
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveHoja((Clave*)(*iterClaves), data);			
		}else{
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
		}
		
		*data = 0;
		
		//Grabar el buffer en el archivo.
		buffer = punteroAux;
		pipe->escribir(buffer);
		
		//Obtener nueva posicion del bloque en el archivo. 
		unsigned short numBloque = 0;
		pipe->leer(&numBloque);
		 
		//Setear en el nodo la posicion donde se grabo el nodo.
		nodoNuevo->setPosicionEnArchivo(numBloque);		
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		delete[] data;
		
		return resultado;
	}
	
	int IndiceArbolManager::escribirBloque(unsigned short numNodo, BloqueIndice* bloqueModif)
	{
		int resultado = 0;
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueModif);
		
		//Variables de escritura del buffer
		string buffer;
		char* data = new char[this->getTamanioBloque()];
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
		//nodoNuevo->actualizarEspacioLibre(this); //NADIE SABIA.
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_NODO, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		pipe->agregarParametro(numNodo, 3); //Numero de nodo a sobre-escribir.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoNuevo->getNivel();  
		headerNodo.refNodo = nodoNuevo->getRefNodo();
		headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
		//punteroAux apunta al inicio del buffer de datos.
		//Necesario para crear un elemento de tipo string.
		const char* punteroAux = data;
		memcpy(data, &headerNodo, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Obtener la lista de claves
		set = nodoNuevo->getClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
		if(nodoNuevo->getNivel() == 0){
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveHoja((Clave*)(*iterClaves), data);
			
		}else{
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
		}
		
		*data = 0;
		//Grabar el buffer en el archivo.
		buffer = punteroAux;
		pipe->escribir(buffer);
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		delete[] data;
		
		return resultado;
	}
	
	int IndiceArbolManager::eliminarBloque(unsigned short posicion)
	{
		char resultado = 0;
	
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros para inicializar el pipe.
		pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_NODO, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
		pipe->agregarParametro(posicion, 2); //Posicion del bloque a eliminar.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		return resultado;
	}
	
	int IndiceArbolManager::escribirBloqueDoble(BloqueIndice* bloqueNuevo)
	{
		int resultado = 0;
		unsigned short numBloque = 0;
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);
		
		//Variables de escritura del buffer
		string buffer;
		char* data = new char[2*this->getTamanioBloque()];
		
		char* primerBloque = new char[this->getTamanioBloque() + 1];
		char* segundoBloque = new char[this->getTamanioBloque() + 1];		
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_NODO_DOBLE, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoNuevo->getNivel();  
		headerNodo.refNodo = nodoNuevo->getRefNodo();
		headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
		//punteroAux apunta al inicio del buffer de datos.
		//Necesario para crear un elemento de tipo string.
		const char* punteroAux = primerBloque;
		memcpy(primerBloque, &headerNodo, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Obtener la lista de claves
		set = nodoNuevo->getClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
		if(nodoNuevo->getNivel() == 0){
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveHoja((Clave*)(*iterClaves), data);
			
		}else{
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
		}
		
		memcpy(primerBloque, punteroAux, this->getTamanioBloque());
		memcpy(segundoBloque, punteroAux + this->getTamanioBloque(), this->getTamanioBloque());
		
		*(primerBloque + this->getTamanioBloque() + 1) = 0;
		*(segundoBloque + this->getTamanioBloque() + 1) = 0;
		
		//Grabar el primer bloque en el archivo.
		buffer = primerBloque;		
		pipe->escribir(buffer);
		
		//Obtener nueva posicion del primer nodo en el archivo
		//y settearla en el nodo. La segunda posicion es contigua.
		pipe->leer(&numBloque);		
		nodoNuevo->setPosicionEnArchivo(numBloque);
		
		//Grabar el segundo bloque en el archivo.
		buffer = segundoBloque;
		pipe->escribir(buffer);
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		
		delete[] data;
		delete[] primerBloque;
		delete[] segundoBloque;
		
		return resultado;

	}
	
	int IndiceArbolManager::escribirBloqueDoble(BloqueIndice* bloqueNuevo, unsigned short numBloque)
	{
		int resultado = 0;
		unsigned short numBloque = 0;
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);
		
		//Variables de escritura del buffer
		string buffer;
		char* data = new char[2*this->getTamanioBloque()];
		
		char* primerBloque = new char[this->getTamanioBloque() + 1];
		char* segundoBloque = new char[this->getTamanioBloque() + 1];		
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica.
		pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_NODO_DOBLE, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		pipe->agregarParametro(numBloque, 3); //Numero de bloque dentro del archivo.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Copiar el header al buffer
		headerNodo.nivel = nodoNuevo->getNivel();  
		headerNodo.refNodo = nodoNuevo->getRefNodo();
		headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
		//punteroAux apunta al inicio del buffer de datos.
		//Necesario para crear un elemento de tipo string.
		const char* punteroAux = primerBloque;
		memcpy(primerBloque, &headerNodo, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Obtener la lista de claves
		set = nodoNuevo->getClaves();
		
		//Recorrer la lista de claves copiando cada clave al buffer
		if(nodoNuevo->getNivel() == 0){
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveHoja((Clave*)(*iterClaves), data);
			
		}else{
			for(SetClaves::iterator iterClaves = set->begin(); 
				iterClaves != set->end(); ++iterClaves)
				this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
		}
		
		memcpy(primerBloque, punteroAux, this->getTamanioBloque());
		memcpy(segundoBloque, punteroAux + this->getTamanioBloque(), this->getTamanioBloque());
		
		*(primerBloque + this->getTamanioBloque() + 1) = 0;
		*(segundoBloque + this->getTamanioBloque() + 1) = 0;
		
		//Grabar el primer bloque en el archivo.
		buffer = primerBloque;		
		pipe->escribir(buffer);
		
		//Grabar el segundo bloque en el archivo.
		buffer = segundoBloque;
		pipe->escribir(buffer);
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		
		delete[] data;
		delete[] primerBloque;
		delete[] segundoBloque;
		
		return resultado;
	}
	
	int IndiceArbolManager::leerBloqueDoble(unsigned int numeroBloque, BloqueIndice* bloqueLeido)
	{
		int resultado = 0;
		Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);
		string buffer;
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;		
		Clave* claveNueva;
		SetClaves* set = new SetClaves();
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros de inicializacion de la Capa Fisica para
		//leer un nodo de disco.
		pipe->agregarParametro(OperacionesCapas::FISICA_LEER_NODO_DOBLE, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
		pipe->agregarParametro(numBloque, 3); //Numero de nodo a leer dentro del archivo
	
		//Se lanza el proceso de la capa fisica.		
		pipe->lanzar();		
		
		//Se obtiene en buffer el contenido de ambos nodos solicitado.
		pipe->leer(this->getTamanioBloque(), buffer);

		//Castear el header
		memcpy(&headerNodo, data, sizeof(HeaderNodo));
		data += sizeof(HeaderNodo);
		
		//Setear el espacio libre, si es hoja y el HijoIzq
		nodoLeido->setNivel(headerNodo.nivel);
		nodoLeido->setEspacioLibre(headerNodo.espacioLibre); 
		nodoLeido->setRefNodo(headerNodo.refNodo);
		
		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.			
		const char* punteroFinal = data + (this->getTamanioBloque() - headerNodo.espacioLibre);
		
		if(nodoLeido->getNivel() == 0){
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = this->leerClaveHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}	 
		}else{
			while(data < punteroFinal){	
				//Leer la clave	
				claveNueva = this->leerClaveNoHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}
		}
		
		//Agregar el setClaves al nodo
		nodoLeido->setClaves(set);
		//Setear la posicion del nodo en el archivo
		nodoLeido->setPosicionEnArchivo(numBloque);	
		
		pipe->leer(&resultado);		
		pipe->liberarRecursos();
		
		delete[] data;
		
		return resultado;
		
	}
	
	int IndiceArbolManager::eliminarBloqueDoble(unsigned short posicion)
	{
		char resultado = 0;
	
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
		
		//Parametros para inicializar el pipe.
		pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_NODO_DOBLE, 0); //Codigo de operacion.
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
		pipe->agregarParametro(posicion, 2); //Posicion del primer bloque a eliminar.
		
		//Se lanza el proceso de la capa fisica. 
		pipe->lanzar();
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		return resultado;
	}
	
	void IndiceArbolManager::exportar(ostream &archivoTexto, int posicion)
	{
		//TODO Revisar implementacion
		/*
		SetClaves* listaClaves;
		Clave* claveLeer;
		//Leo el nodo
		
		Nodo* nodo = new Nodo(this,posicion);
	
		//Imprime el header del nodo	
		archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
		listaClaves = nodo->obtenerClaves();
		archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
		archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
		
		//Recorro la lista de claves
		listaClaves->primero();
		while(!listaClaves->fin()){
			claveLeer = static_cast<Clave*>(listaClaves->obtenerDato());
			claveLeer->imprimir(archivoTexto);
			listaClaves->siguiente();	
		}
		
		archivoTexto<<endl;
		delete nodo;
		*/
	
	}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceHashManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de dispersion de la capa de indices).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		IndiceHashManager::IndiceHashManager(unsigned int tamBucket, string nombreArchivo):
			IndiceManager(tamBucket, nombreArchivo, TipoIndices::HASH)
		{ }
		
		IndiceHashManager::~IndiceHashManager() { }
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		int IndiceHashManager::leerBloque(unsigned int numBucket, BloqueIndice* bloqueLeido)
		{
			char resultado = 0;
			Bucket* bucketLeido = static_cast<Bucket*> (bloqueLeido);
			string buffer;
			
			//Variable de interpretacion del bucket
			HeaderBucket headerBucket;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de ejecucion de la Capa Fisica para leer
			//un bucket de disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_BUCKET, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket.
			pipe->agregarParametro(numBucket, 3); //Numero de bucket a leer.
		
			//Se lanza el proceso de la capa fisica. 
			//Se obtiene en buffer el contenido del Bucket solicitado.
			pipe->lanzar();
			pipe->leer(this->getTamanioBloque(), buffer);
			
			char * data = (char*) buffer.c_str();
			
			bucketLeido->setDatos(data);
			memcpy(&headerBucket, data, sizeof(HeaderBucket));
			
			bucketLeido->setTamDispersion(headerBucket.tamDispersion);
			bucketLeido->setEspacioLibre(headerBucket.espLibre);
			bucketLeido->setCantRegs(headerBucket.cantRegs);
			bucketLeido->setNroBloque(numBucket);
		
			pipe->leer(&resultado);		
			pipe->liberarRecursos();
			
			return resultado;						
		}
		
		int IndiceHashManager::escribirBloque(BloqueIndice* nuevoBloque)
		{
			char resultado = 0;
			Bucket* bucketLeido = static_cast<Bucket*> (nuevoBloque);
			
			//Variables de escritura del buffer
			string buffer;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de ejecucion de la Capa Fisica para escribir un
			//bucket a disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_BUCKET, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//TODO Chequear que getDatos() devuelve el char* tal cual se va a meter a disco.
			
			char* data = bucketLeido->getDatos();
			*(data + this->getTamanioBloque() + 1) = 0;
			
			//Grabar el buffer en el archivo.
			buffer = data;
			pipe->escribir(buffer);
			
			//Obtener nueva posicion del bucket en el archivo. 
			unsigned short numBucket = 0;
			pipe->leer(&numBucket);
			 
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
			
			pipe->liberarRecursos();
			delete[] data;
			
			return resultado;
		}
		
		int IndiceHashManager::escribirBloque(unsigned short numBucket, BloqueIndice* bloqueModif)
		{
			char resultado = 0;
			Bucket* bucketLeido = static_cast<Bucket*> (bloqueModif);
			
			//Variables de escritura del buffer
			string buffer;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de ejecucion de la Capa Fisica para modificar
			//un bucket en disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_BUCKET, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket en disco.
			pipe->agregarParametro(numBucket, 3); //Numero de bucket a sobre-escribir.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//TODO Chequear que getDatos() devuelve el char* tal cual se va a meter a disco.
			
			char* data = bucketLeido->getDatos();
			*(data + this->getTamanioBloque() + 1) = 0;
			
			//Grabar el buffer en el archivo.
			buffer = data;
			pipe->escribir(buffer);
	
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
			
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
			
			pipe->liberarRecursos();
			delete[] data;
			
			return resultado;
		}
		
		int IndiceHashManager::eliminarBloque(unsigned short numBucket)
		{
			char resultado = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//eliminar un bucket de disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_BUCKET, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket en disco
			pipe->agregarParametro(numBucket, 2); //Numero de bucket a eliminar dentro del archivo.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
			
			return resultado;
		}
		
		void IndiceHashManager::leerTabla(unsigned int* tamanio, unsigned int* buckets)
		{
			string bucketsTabla;
						
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de inicializacion de la Capa Fisica para
			//leer la tabla de dispersion 
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_TABLA_HASH, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Obtener tamaño de la tabla
			pipe->leer(tamanio);
			
			//Obtener numero de buckets de la tabla
			pipe->leer(sizeof(unsigned int)*(*tamanio), bucketsTabla);
			buckets = (unsigned int*) bucketsTabla.c_str();
			
			pipe->liberarRecursos();
		}
		
		void IndiceHashManager::escribirTabla(unsigned int tamanio, unsigned int* buckets)
		{
			string bucketsTabla;
									
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(NOMBRE_CAPA_FISICA);
			
			//Parametros de inicializacion de la Capa Fisica para
			//actualizar la tabla de dispersion 
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_TABLA_HASH, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Enviar el tamaño de la tabla por el pipe.
			pipe->escribir(tamanio);
			
			bucketsTabla = (char*) buckets;
			
			//Enviar el contenido de la tabla por el pipe.
			pipe->escribir(bucketsTabla);
			
			pipe->liberarRecursos();			
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceSecundario
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices secundarios de la capa de indices).
///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
	IndiceSecundarioManager::IndiceSecundarioManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
		IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
	{			 
			this->tamanioArray = tamBloqueLista/sizeof(int); 
	}
	
	IndiceSecundarioManager::~IndiceSecundarioManager(){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	void IndiceSecundarioManager::sobreEscribirListaClavesP(unsigned int posicion, SetClaves* setClaves)
	{
		/*
		//Variables
		int arrayRef[this->tamanioArray]; 
		int arrayAnt[this->tamanioArray];
		int i = 0;
		int parciales = 0;
		bool fin = false;
		bool haySiguiente;
		int posicionSiguiente = posicion;
		
		//Alamceno la cantidad de claves
		int totales = listaClaves->getCantidadNodos();
		 
		listaClaves->primero();
		while(!fin){
				//Se posiciona en el primer registro de la lista a sobreescribir
				this->archivoLista->posicionarse(posicionSiguiente);
				
				while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
					i++;
					parciales++;
					arrayRef[i] = *((int*)listaClaves->obtenerDato());
					listaClaves->siguiente();
				}
				//Almaceno la cantidad de claves de la lista
				arrayRef[0] = i;
				
				//Chequeo si tengo que seguir escribiendo en otro bloque
				if(parciales<totales){
					
					//Obtener la proxima posicion donde leer en el archivo, si es que
					// hay definida una
					if(!this->archivoLista->fin()){
						this->archivoLista->leer(arrayAnt);
						this->cantLecturasLista++;
						this->archivoLista->posicionarse(archivoLista->posicion()-1);
						if(arrayAnt[tamanioArray-1]!=-1)
							haySiguiente = true;
						else
							haySiguiente = false;
					}
					else
						haySiguiente = false;
						
					if(haySiguiente)
						//Caso en el que ya esta definido donde debe seguir la lista
						posicionSiguiente = arrayAnt[tamanioArray-1];
					
					else{
						//Si no hay otro bloque definido sigo al final del archivo
						int posBck = this->archivoLista->posicion();
						this->archivoLista->posicionarseFin();
						posicionSiguiente = this->archivoLista->posicion();
						this->archivoLista->posicionarse(posBck); 			
					}								
				}		
				else{
					//Almaceno que no hay mas bloques de la lista
					posicionSiguiente = -1; 
					fin = true;
				}
				//Almaceno el bloque donde se va a continuar escribiendo
				arrayRef[tamanioArray-1] = posicionSiguiente;
				//Escribo el bloque en el archivo
				this->archivoLista->escribir(arrayRef);
				this->cantEscriturasLista++;
				//Se posiciona en el proximo registro donde escribir lo que falta de lista
				i = 0;	
		}
		*/
	}
	
	SetClaves* IndiceSecundarioManager::obtenerLstClavesP(unsigned int posicion)
	{
		//TODO Revisar implementacion.
		
		/*
		//Variables
		Lista* listaClaves = new Lista;
		int arrayRef[this->tamanioArray];
		int cantClaves;
		bool fin = false;
		
		//Se posiciona al final del archivo
		this->archivoLista->posicionarse(posicion);
		
		while(!fin){
			
			//Obtengo la cantidad de claves
			this->archivoLista->leer(arrayRef);
			this->cantLecturasLista++;
			cantClaves = arrayRef[0];
				
			for(int i=0;i<cantClaves;i++)
				listaClaves->agregar(new int(arrayRef[i+1]));
			
			if(arrayRef[tamanioArray-1]!=-1)
				archivoLista->posicionarse(arrayRef[tamanioArray-1]);
			else
				fin = true;			
		}
		
		return listaClaves;
		*/
		
		return NULL;
	}

	unsigned int IndiceSecundarioManager::grabarNuevaLstClavesP(SetClaves* setClaves)
	{
		/*
		//Variables
		int arrayRef[this->tamanioArray]; 
		int i = 0;
		int parciales = 0;
		bool fin = false;
		
		//Me posiciono al final del archivo
		this->archivoLista->posicionarseFin();
		
		//Almaceno la posicion donde se grabo para devolverla
		int posicion = this->archivoLista->posicion();
		
		//Almaceno la cantidad de claves
		int totales = listaClaves->getCantidadNodos();
		 
		listaClaves->primero();
		while(!fin){
				
				while( (i<tamanioArray-2) && (!listaClaves->fin()) ){
					i++;
					parciales++;
					arrayRef[i] = *((int*)listaClaves->obtenerDato());
					listaClaves->siguiente();
				}
				//Almaceno la cantidad de claves de la lista		
				arrayRef[0] = i;
				
				//Chequeo si tengo que seguir escribiendo en otro bloque
				if(parciales<totales){
					//Almaceno el bloque donde se va a continuar escribiendo
					arrayRef[tamanioArray-1] = this->archivoLista->posicion()+1;	
				}		
				else{
					//Almaceno que no hay un bloque donde continuar
					arrayRef[tamanioArray-1] = -1; 
					fin = true;
				}
				//Escribo el bloque en el archivo
				this->archivoLista->escribir(arrayRef);
				this->cantEscriturasLista++;
				i = 0;
		}
		
		return posicion;
		*/
		
		return 0;
	}
	
	void IndiceSecundarioManager::exportar(ostream &archivoTexto,int posicion)
	{
		/*
		ListaClaves* listaClaves;
		Lista* listaClavesPrim;
		ClaveCadena* claveLeer; 
		//Leo el nodo
		
		Nodo* nodo = new Nodo(this,posicion);
	
		//Imprime el header del nodo
		archivoTexto<<"Pos. de Archivo: "<<posicion<<endl;
		listaClaves = nodo->obtenerClaves();
		archivoTexto<<"Hijo Izquierdo: "<<nodo->getHijoIzq()<<endl;
		archivoTexto<<"Nivel: "<<nodo->getNivel()<<endl;
		
		//Recorro la lista de claves
		listaClaves->primero();
		while(!listaClaves->fin()){
			claveLeer = static_cast<ClaveCadena*>(listaClaves->obtenerDato());
			claveLeer->imprimir(archivoTexto);
			
			archivoTexto<<"Lista de Claves Primarias:"<<endl;
			//Leer la lista de claves primarias del nombre
			listaClavesPrim = this->obtenerLstClavesP(claveLeer->getRefListaPrim());
			
			//Recorrer la lista de claves primarias imprimiendo
			listaClavesPrim->primero();
			while(!listaClavesPrim->fin()){
				archivoTexto<<*(int*)listaClavesPrim->obtenerDato()<<endl;
				listaClavesPrim->siguiente();	
			}
			
			//Borrar la lista
			listaClavesPrim->primero();
			while(!listaClavesPrim->fin()){
				delete static_cast<int*>(listaClavesPrim->obtenerDato());
				listaClavesPrim->siguiente();	
			}
			delete listaClavesPrim;
			
			
			listaClaves->siguiente();	
		}
		archivoTexto<<"------------------------------"<<endl;
		delete nodo;
		
		*/
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceEnteroGriegoManager 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
	IndiceEnteroGriegoManager::IndiceEnteroGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
		IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceEnteroGriegoManager::leerClaveHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer+=sizeof(int);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
		
		Clave* IndiceEnteroGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer+=sizeof(int);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveEntera(valor, refRegistro, hijoDer);
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceInteroRomanoManager
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	//////////////////////////////////////////////////////////////////////
		IndiceEnteroRomanoManager::IndiceEnteroRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
		
		IndiceEnteroRomanoManager::~IndiceEnteroRomanoManager()
		{ }	
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		Clave* IndiceEnteroRomanoManager::leerClaveHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer += sizeof(int);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
		
		Clave* IndiceEnteroRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer += sizeof(int);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveEntera(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceBooleanGriegoManager
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceBooleanGriegoManager::IndiceBooleanGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////			
		Clave* IndiceBooleanGriegoManager::leerClaveHoja(char* &buffer)
		{
			bool valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(bool));
			buffer += sizeof(bool);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
			
		Clave* IndiceBooleanGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			bool valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor booleano de la clave.
			memcpy(&valor, buffer, sizeof(bool));
			buffer += sizeof(bool);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveBoolean(valor, refRegistro, hijoDer);
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCharGriegoManager
//		   (Implementa archivo de indices primarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCharGriegoManager::IndiceCharGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		Clave* IndiceCharGriegoManager::leerClaveHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveChar(valor, refRegistro);	
		}
				
		Clave* IndiceCharGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor de tipo char de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveChar(valor, refRegistro, hijoDer);
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCharRomanoManager
//		   (Implementa archivo de indices secundarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////	
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCharRomanoManager::IndiceCharRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceCharRomanoManager::leerClaveHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveChar(valor, refRegistro);	
		}
						
		Clave* IndiceCharRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor de tipo char de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveChar(valor, refRegistro, hijoDer);
		}
	
//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nombre: IndiceShortGriegoManager
//		   (Implementa archivo de indices primarios de clave de tipo short).
//////////////////////////////////////////////////////////////////////////////	
					
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceShortGriegoManager::IndiceShortGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{ }
				
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceShortGriegoManager::leerClaveHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveShort(valor, refRegistro);	
		}
						
		Clave* IndiceShortGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveShort(valor, refRegistro, hijoDer);
		}
				
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceShortRomanoManager
//		   (Implementa archivo de indices secundarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceShortRomanoManager::IndiceShortRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceShortRomanoManager::leerClaveHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveShort(valor, refRegistro);	
		}
						
		Clave* IndiceShortRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear clave
			return new ClaveShort(valor, refRegistro, hijoDer);
		}

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceRealRomanoManager
//		   (Implementa archivo de indices secundarios de clave de tipo float).
//////////////////////////////////////////////////////////////////////////////
							
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceRealRomanoManager::IndiceRealRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceRealRomanoManager::leerClaveHoja(char* &buffer)
		{
			float valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(float));
			buffer += sizeof(float);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveReal(valor, refRegistro);	
		}
						
		Clave* IndiceRealRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			float valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(float));
			buffer += sizeof(float);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveReal(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaGriegoManager
//		   (Implementa archivo de indices primarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
							
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceFechaGriegoManager::IndiceFechaGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceFechaGriegoManager::leerClaveHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro);	
		}
						
		Clave* IndiceFechaGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor fecha de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro, hijoDer);
		}

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaRomanoManager
//		   (Implementa archivo de indices secundarios de clave de tipo fecha).
//////////////////////////////////////////////////////////////////////////////
									
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceFechaRomanoManager::IndiceFechaRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
								
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* IndiceFechaRomanoManager::leerClaveHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro);	
		}
						
		Clave* IndiceFechaRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor fecha de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro, hijoDer);
		}

		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceVariableGriegoManager 
//		   (Implementa archivo de indices primarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceVariableGriegoManager::IndiceVariableGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	

		void IndiceVariableGriegoManager::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		} 
			
		void IndiceVariableGriegoManager::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
		}	
			
			
		Clave* IndiceVariableGriegoManager::leerClaveHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* IndiceVariableGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveVariable(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceVariableRomanoManager 
//		   (Implementa archivo de indices secundarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceVariableRomanoManager::IndiceVariableRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void IndiceVariableRomanoManager::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		} 
			
		void IndiceVariableRomanoManager::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			string* valor = (string*) clave->getValor();
			
			//Copia del valor entero de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			}
		}	
	
		Clave* IndiceVariableRomanoManager::leerClaveHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* IndiceVariableRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveVariable(valor, refRegistro, hijoDer);
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCompuestoGriegoManager 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCompuestoGriegoManager::IndiceCompuestoGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice, ListaTipos* listaTipos):
			IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
		{
			this->tipos = listaTipos;
		}
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		
		void IndiceCompuestoGriegoManager::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
			ListaClaves* listaClaves = claveCompuesta->getListaClaves();
			Clave* claveAux;
			
			for (ListaClaves::const_iterator iterClaves = listaClaves->begin();
				iterClaves != listaClaves->end(); ++iterClaves) {
				
				claveAux = (*iterClaves);
				//Copia del valor de la clave
				memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
				buffer += claveAux->getTamanioValor();
			}
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		}
			
		void IndiceCompuestoGriegoManager::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
			ListaClaves* listaClaves = claveCompuesta->getListaClaves();
			Clave* claveAux;
			
			//Copia de los valores de las claves en el nodo
			for (ListaClaves::const_iterator iterClaves = listaClaves->begin();
				iterClaves != listaClaves->end(); ++iterClaves) {
				
				claveAux = (*iterClaves);
				//Copia del valor de la clave
				memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
				buffer += claveAux->getTamanioValor();
			}
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Copia de la referencia al hijo derecho.
			referencia = clave->getHijoDer();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);			
		}			
			
		Clave* IndiceCompuestoGriegoManager::leerClaveHoja(char* &buffer)
		{
			ListaClaves listaClaves;
			int tipo;
			unsigned int refRegistro = 0, tamanio = 0;
			void* valor = NULL;
			
			//Copia de los valores de las claves en el nodo				
			for (ListaTipos::const_iterator iterTipos = this->tipos->begin();
				iterTipos != this->tipos->end(); ++iterTipos) {			
					
					tipo = (*iterTipos);
					
					if (tipo == TipoDatos::TIPO_ENTERO){						
						//Copia del valor de la clave
						valor = new int;
						tamanio = sizeof(int);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveEntera(*((int*)valor)));
					
					}else if (tipo == TipoDatos::TIPO_BOOL){
						valor = new bool;
						tamanio = sizeof(bool);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
					}else if (tipo == TipoDatos::TIPO_CHAR){
						valor = new char;
						tamanio = sizeof(char);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveChar(*((char*)valor)));
						
					}else if (tipo == TipoDatos::TIPO_SHORT){
						valor = new short;
						tamanio = sizeof(short);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveShort(*((short*)valor)));
						
					}else if (tipo == TipoDatos::TIPO_FLOAT){
						valor = new float;
						tamanio = sizeof(float);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveReal(*((float*)valor)));
					}else if (tipo == TipoDatos::TIPO_FECHA){
						valor = new ClaveFecha::TFECHA;
						tamanio = sizeof(ClaveFecha::TFECHA);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveFecha((ClaveFecha::TFECHA*)valor));
					}else if (tipo == TipoDatos::TIPO_VARIABLE){
						valor = new string;
						valor = buffer;
						tamanio = ((string*)valor)->size() + 1;
	
						listaClaves.push_back(new ClaveVariable(*((string*)valor)));
					}
					
					buffer += tamanio;
				}
	
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveCompuesta(listaClaves, refRegistro);	
		}
							
		Clave* IndiceCompuestoGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			ListaClaves listaClaves;
			int tipo;
			unsigned int refRegistro = 0, hijoDer = 0, tamanio = 0;
			void* valor = NULL;
			
			//Copia de los valores de las claves en el nodo				
			for (ListaTipos::const_iterator iterTipos = this->tipos->begin();
				iterTipos != this->tipos->end(); ++iterTipos) {			
					
				tipo = (*iterTipos);
				
				if (tipo == TipoDatos::TIPO_ENTERO){						
					//Copia del valor de la clave
					valor = new int;
					tamanio = sizeof(int);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveEntera(*((int*)valor)));
				
				}else if (tipo == TipoDatos::TIPO_BOOL){
					valor = new bool;
					tamanio = sizeof(bool);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
				}else if (tipo == TipoDatos::TIPO_CHAR){
					valor = new char;
					tamanio = sizeof(char);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveChar(*((char*)valor)));
					
				}else if (tipo == TipoDatos::TIPO_SHORT){
					valor = new short;
					tamanio = sizeof(short);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveShort(*((short*)valor)));
					
				}else if (tipo == TipoDatos::TIPO_FLOAT){
					valor = new float;
					tamanio = sizeof(float);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveReal(*((float*)valor)));
				}else if (tipo == TipoDatos::TIPO_FECHA){
					valor = new ClaveFecha::TFECHA;
					tamanio = sizeof(ClaveFecha::TFECHA);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveFecha((ClaveFecha::TFECHA*)valor));
				}else if (tipo == TipoDatos::TIPO_VARIABLE){
					valor = new string;
					valor = buffer;
					tamanio = ((string*)valor)->size() + 1;
				
					listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				}
					
				buffer += tamanio;
			}			
	
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveCompuesta(listaClaves, refRegistro);
		}


