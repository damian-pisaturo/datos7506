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
		char resultado = 0;
		Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);
		
		//Buffer
		char* data = new char[this->getTamanioBloque() + 1];
		char* punteroAux = data;
		
		//Variables de interpretacion del nodo		
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
		
		pipe->leer(this->getTamanioBloque(), data);		
		
		//Castear el header
		unsigned char nivel;
		unsigned int refNodo;
		unsigned short espLibre;
		
		memcpy(&nivel, data, sizeof(unsigned char));
		data += sizeof(unsigned char);
		
		memcpy(&refNodo, data, sizeof(unsigned int));
		data += sizeof(unsigned int);
		
		memcpy(&espLibre, data, sizeof(unsigned short));
		data += sizeof(unsigned short);
			
		//memcpy(&headerNodo, data, sizeof(HeaderNodo));
		//data += sizeof(HeaderNodo);
		//HeaderNodo headerNodo;			
		
		//Setear el espacio libre, si es hoja y el HijoIzq
		nodoLeido->setNivel(nivel);
		nodoLeido->setEspacioLibre(espLibre); 
		nodoLeido->setRefNodo(refNodo);
		
		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.
		const char* punteroFinal = punteroAux + (this->getTamanioBloque() - espLibre - nodoLeido->getTamanioHeader());
		
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
		
		delete[] punteroAux;
		
		return resultado;
	}
	
	int IndiceArbolManager::escribirBloque(BloqueIndice* bloqueNuevo)
	{	
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);
		
		//Variables de escritura del buffer
		char* data = new char[this->getTamanioBloque()];
		char* punteroAux = data;
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
		//Instancia del pipe
		ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);

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
		
		memcpy(data, &headerNodo.nivel, sizeof(char));
		data+=sizeof(char);
		memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
		data+=sizeof(unsigned int);
		memcpy(data, &headerNodo.espacioLibre, sizeof(unsigned short));		
		data+=sizeof(unsigned short);
		
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
		
		//Grabar el buffer en el archivo.
		pipe->escribir(punteroAux, this->getTamanioBloque());
		
		//Obtener nueva posicion del bloque en el archivo. 
		unsigned short numBloque = 0;
		pipe->leer(&numBloque);
		 
		//Setear en el nodo la posicion donde se grabo el nodo.
		nodoNuevo->setPosicionEnArchivo(numBloque);		
		
		pipe->liberarRecursos();
		delete[] punteroAux;
	
		return numBloque;
	}
	
	int IndiceArbolManager::escribirBloque(unsigned short numNodo, BloqueIndice* bloqueModif)
	{
		int resultado = 0;
		Nodo* nodoNuevo = static_cast<Nodo*> (bloqueModif);
		
		//Variables de escritura del buffer
		char* data = new char[this->getTamanioBloque() + 1];
		const char* punteroAux = data;
		
		//Variables de interpretacion del nodo
		HeaderNodo headerNodo;
		SetClaves* set;
		
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
		
		//Grabar el buffer en el archivo.
		pipe->escribir(punteroAux, this->getTamanioBloque());
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		delete pipe;
		delete[] punteroAux;
		
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
		
		memcpy(&headerNodo.nivel, data, sizeof(unsigned char));
		data += sizeof(unsigned char);
				
		memcpy(&headerNodo.refNodo, data, sizeof(unsigned int));
		data += sizeof(unsigned int);
		
		memcpy(&headerNodo.espacioLibre, data, sizeof(unsigned short));
		data += sizeof(unsigned short);

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
		//pipe->escribir(buffer);
		
		//Obtener nueva posicion del primer nodo en el archivo
		//y settearla en el nodo. La segunda posicion es contigua.
		pipe->leer(&numBloque);		
		nodoNuevo->setPosicionEnArchivo(numBloque);
		
		//Grabar el segundo bloque en el archivo.
		buffer = segundoBloque;
		//pipe->escribir(buffer);
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		
		delete[] data;
		delete[] primerBloque;
		delete[] segundoBloque;
		
		return resultado;

	}
	
	int IndiceArbolManager::escribirBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueNuevo)
	{
		int resultado = 0;
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
		//pipe->escribir(buffer);
		
		//Grabar el segundo bloque en el archivo.
		buffer = segundoBloque;
		//pipe->escribir(buffer);
		
		//Solicitar resultado de la comunicacion con la 
		//capa fisica.
		pipe->leer(&resultado);
		
		pipe->liberarRecursos();
		
		delete[] data;
		delete[] primerBloque;
		delete[] segundoBloque;
		
		return resultado;
	}
	
	int IndiceArbolManager::leerBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueLeido)
	{
		int resultado = 0;
		Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);
		string buffer;
		char* data = NULL;
		
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
		//pipe->leer(this->getTamanioBloque(), buffer);
		
		data = (char*)buffer.c_str();
		
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
			char* buffer = new char[this->getTamanioBloque() + 1];
			
			//Variable de interpretacion del bucket
			HeaderBucket headerBucket;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto");
			
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
			
			bucketLeido->setDatos(buffer);
			
			memcpy(&headerBucket.espLibre,buffer,Tamanios::TAMANIO_ESPACIO_LIBRE);
			buffer += Tamanios::TAMANIO_ESPACIO_LIBRE;
			memcpy(&headerBucket.cantRegs,buffer, Tamanios::TAMANIO_CANTIDAD_REGISTROS);
			buffer += Tamanios::TAMANIO_CANTIDAD_REGISTROS;
			memcpy(&headerBucket.tamDispersion, buffer, Tamanios::TAMANIO_DISPERSION);
			
			bucketLeido->setTamDispersion(headerBucket.tamDispersion);
			bucketLeido->setEspacioLibre(headerBucket.espLibre);
			bucketLeido->setCantRegs(headerBucket.cantRegs);
			bucketLeido->setNroBloque(numBucket);
			
			pipe->leer(&resultado);
			
			delete pipe;
			return resultado;						
		}
		
		int IndiceHashManager::escribirBloque(BloqueIndice* nuevoBloque)
		{
			char resultado = 0;
			Bucket* bucketLeido = static_cast<Bucket*> (nuevoBloque);
			
			//Variable de escritura del buffer
			char* data = NULL;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(/*NOMBRE_CAPA_FISICA*/"ernesto");
			
			//Parametros de ejecucion de la Capa Fisica para escribir un
			//bucket a disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_BUCKET, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			data = bucketLeido->getDatos();
			
			//Grabar el buffer en el archivo
			pipe->escribir(data, this->getTamanioBloque());
			
			//Obtener nueva posicion del bucket en el archivo. 
			unsigned short numBucket = 0;
			pipe->leer(&numBucket);
			 
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
			
			pipe->liberarRecursos();
			
			return resultado;
		}
		
		int IndiceHashManager::escribirBloque(unsigned short numBucket, BloqueIndice* bloqueModif)
		{
			char resultado = 0;
			Bucket* bucketLeido = static_cast<Bucket*> (bloqueModif);
			
			//Variables de escritura del buffer
			char* buffer = NULL;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(/*NOMBRE_CAPA_FISICA*/"ernesto");
			
			//Parametros de ejecucion de la Capa Fisica para modificar
			//un bucket en disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_BUCKET, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket en disco.
			pipe->agregarParametro(numBucket, 3); //Numero de bucket a sobre-escribir.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			buffer = bucketLeido->getDatos();
			
			//Grabar el buffer en el archivo.
			pipe->escribir(buffer, this->getTamanioBloque());
	
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
			
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
			
			pipe->liberarRecursos();
			
			return resultado;
		}
		
		int IndiceHashManager::eliminarBloque(unsigned short numBucket)
		{
			char resultado = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
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
		
		void IndiceHashManager::leerTabla(unsigned int* tamanio, unsigned int* &buckets)
		{
			char* bucketsTabla = NULL;
						
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto");
			
			//Parametros de inicializacion de la Capa Fisica para
			//leer la tabla de dispersion 
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_TABLA_HASH, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Obtener tamaño de la tabla
			pipe->leer(tamanio);			
			
			if(tamanio > 0) {
				unsigned int sizeTabla = (sizeof(unsigned int))*(*tamanio);

				bucketsTabla = new char[sizeTabla];
				
				//Obtener datos de la tabla				
				pipe->leer(sizeTabla, bucketsTabla);				
			}
			
			buckets = (unsigned int*) bucketsTabla;				
			delete pipe;
			
			cout << "deleteo el pipe"<< endl;
			cout <<"contenido buckets: "<< buckets[0]<< endl;
		}
		
		void IndiceHashManager::escribirTabla(unsigned int tamanio, unsigned int* buckets)
		{
			char* bucketsTabla = NULL;
								
			cout<< "tamanio: "<< tamanio<<endl;
			cout << "contenido[0]: "<< buckets[0]<<endl;
			
			cout << "contenido[1]: "<< buckets[1]<<endl;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe(/*NOMBRE_CAPA_FISICA*/"ernesto");
			
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
			cout << "tamanioBloque: "<< this->getTamanioBloque()<<endl;
			
			pipe->escribir(bucketsTabla, this->getTamanioBloque());
			
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
			this->tamBloqueLista = tamBloqueLista; 
	}
	
	IndiceSecundarioManager::~IndiceSecundarioManager(){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	char IndiceSecundarioManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves)
	{		
		char resultado         = 0;
		char* bufferClaves     = NULL; 
		ComuDatos* pipe        = NULL;
		
		if (setClaves->size() > 0){
			//Instancia del pipe
			pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 2); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			bufferClaves = new char[this->getTamanioBloqueLista()];		
			unsigned int offset = 0;
			
			for (SetClaves::iterator it = setClaves->begin();  it != setClaves->end(); ++it){
				memcpy(bufferClaves + offset, (*it)->getValor(),(*it)->getTamanioValor());
				offset += (*it)->getTamanioValor();
			}
			
			//Escritura del tamanio de la lista (cantidad de claves a almacenar)
			pipe->escribir(setClaves->size());
			
			//Escritura de la lista al archivo.
			pipe->escribir(bufferClaves, this->getTamanioBloqueLista());
			
			//Obtencion del resultado de la operacion
			pipe->leer(&resultado);	
		}
		
		if (pipe)
			delete pipe;
		
		if (bufferClaves)
			delete[] bufferClaves;
		
		return resultado;
	}
	
	unsigned int IndiceSecundarioManager::escribirListaClaves(SetClaves* setClaves)
	{		
		unsigned int numBloque = 0;
		char* bufferClaves     = NULL; 
		ComuDatos* pipe        = NULL;
		
		if (setClaves->size() > 0){
			//Instancia del pipe
			pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
		
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			bufferClaves = new char[this->getTamanioBloqueLista()];		
			unsigned int offset = 0;
			
			for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it){
				memcpy(bufferClaves + offset, (*it)->getValor(),(*it)->getTamanioValor());
				offset += (*it)->getTamanioValor();
			}
			
			//Escritura de la cantidad de claves en la lista
			pipe->escribir(setClaves->size());
			
			//Escritura de la lista al archivo.
			pipe->escribir(bufferClaves, this->getTamanioBloqueLista());
			
			//Obtencion del numero de bloque donde fue insertada la lista.
			pipe->leer(&numBloque);	
		}
		
		if (pipe)
			delete pipe;
		
		if (bufferClaves)
			delete[] bufferClaves;
		
		return numBloque;
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
			buffer+=Tamanios::TAMANIO_REFERENCIA;
		
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
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////

		SetClaves* IndiceEnteroRomanoManager::leerListaClaves(unsigned int posicion)
		{		
			SetClaves* setClaves    = NULL;
			char* bufferClaves      = NULL; 
			unsigned int cantClaves = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 2); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			//Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			if(cantClaves > 0) {
				bufferClaves = new char[this->getTamanioBloqueLista()];
				
				//Leer la lista del archivo.
				pipe->leer(this->getTamanioBloqueLista(), bufferClaves);
				
				setClaves = new SetClaves();
				for (unsigned char i = 0; i < cantClaves; i++)
					setClaves->insert(new ClaveEntera( *((int*)bufferClaves) + i));				
			}
			
			return setClaves;
		}
		
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
		IndiceBooleanRomanoManager::IndiceBooleanRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice):
			IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }

	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		SetClaves* IndiceBooleanRomanoManager::leerListaClaves(unsigned int posicion)
		{
			//TODO 
			return NULL;
		}
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////			
		Clave* IndiceBooleanRomanoManager::leerClaveHoja(char* &buffer)
		{
			bool valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(bool));
			buffer += sizeof(bool);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveBoolean(valor, refRegistro);	
		}
			
		Clave* IndiceBooleanRomanoManager::leerClaveNoHoja(char* &buffer)
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
		
		SetClaves* IndiceCharRomanoManager::leerListaClaves(unsigned int posicion)
		{		
			SetClaves* setClaves    = NULL;
			char* bufferClaves      = NULL; 
			unsigned int cantClaves = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			//Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			if(cantClaves > 0) {
				bufferClaves = new char[this->getTamanioBloqueLista()];
				
				//Leer la lista del archivo.
				pipe->leer(this->getTamanioBloqueLista(), bufferClaves);
				
				setClaves = new SetClaves();
				for (unsigned char i = 0; i < cantClaves; i++)
					setClaves->insert(new ClaveChar(*((char*)bufferClaves) + i));				
			}
			
			return setClaves;
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
			unsigned int refRegistro = 0;		
			unsigned int hijoDer     = 0;
			
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear clave
			return new ClaveShort(valor, refRegistro, hijoDer);
		}
		
		SetClaves* IndiceShortRomanoManager::leerListaClaves(unsigned int posicion)
		{		
			SetClaves* setClaves    = NULL;
			char* bufferClaves      = NULL; 
			unsigned int cantClaves = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 2); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			//Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			if(cantClaves > 0) {
				bufferClaves = new char[this->getTamanioBloqueLista()];
				
				//Leer la lista del archivo.
				pipe->leer(this->getTamanioBloqueLista(), bufferClaves);
				
				setClaves = new SetClaves();
				for (unsigned char i = 0; i < cantClaves; i++)
					setClaves->insert(new ClaveShort(*((short*)bufferClaves) + i));				
			}
			
			return setClaves;
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveReal(valor, refRegistro, hijoDer);
		}
		
		SetClaves* IndiceRealRomanoManager::leerListaClaves(unsigned int posicion)
		{		
			SetClaves* setClaves    = NULL;
			char* bufferClaves      = NULL; 
			unsigned int cantClaves = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 2); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			//Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			if(cantClaves > 0) {
				bufferClaves = new char[this->getTamanioBloqueLista()];
				
				//Leer la lista del archivo.
				pipe->leer(this->getTamanioBloqueLista(), bufferClaves);
				
				setClaves = new SetClaves();
				for (unsigned char i = 0; i < cantClaves; i++)
					setClaves->insert(new ClaveReal(*((float*)bufferClaves) + i));				
			}
			
			return setClaves;
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
		
			//Se interpreta el valor TFECHA de la clave.
			memcpy(&(valor.anio), buffer, sizeof(short));
			buffer += sizeof(short);
			
			memcpy(&(valor.mes), buffer, sizeof(char));
			buffer += sizeof(char);
			
			memcpy(&(valor.dia), buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += sizeof(Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveFecha(valor, refRegistro);	
		}
						
		Clave* IndiceFechaGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor TFECHA de la clave.
			memcpy(&(valor.anio), buffer, sizeof(short));
			buffer += sizeof(short);
			
			memcpy(&(valor.mes), buffer, sizeof(char));
			buffer += sizeof(char);
			
			memcpy(&(valor.dia), buffer, sizeof(char));
			buffer += sizeof(char);
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveFecha(valor, refRegistro, hijoDer);
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
		
			//Se interpreta el valor TFECHA de la clave.
			memcpy(&(valor.anio), buffer, sizeof(short));
			buffer += sizeof(short);
			
			memcpy(&(valor.mes), buffer, sizeof(char));
			buffer += sizeof(char);
			
			memcpy(&(valor.dia), buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveFecha(valor, refRegistro);	
		}
						
		Clave* IndiceFechaRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor TFECHA de la clave.
			memcpy(&(valor.anio), buffer, sizeof(short));
			buffer += sizeof(short);
			
			memcpy(&(valor.mes), buffer, sizeof(char));
			buffer += sizeof(char);
			
			memcpy(&(valor.dia), buffer, sizeof(char));
			buffer += sizeof(char);						
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveFecha(valor, refRegistro, hijoDer);
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
			//Copia de la longitud de la clave.
			unsigned short sizeCadena = clave->getTamanioValor();
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, clave->getValor(), sizeCadena);
			buffer += sizeCadena;
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		} 
			
		void IndiceVariableGriegoManager::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			
			//Copia de la longitud de la clave.
			unsigned short sizeCadena = clave->getTamanioValor();
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, clave->getValor(), sizeCadena);
			buffer += sizeCadena;
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			referencia = clave->getHijoDer();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		}	
			
			
		Clave* IndiceVariableGriegoManager::leerClaveHoja(char* &buffer)
		{
			string valor;
			char* cadena = NULL;
			unsigned int refRegistro = 0;
			
			//Se interpreta la longitud del valor de la clave variable.
			unsigned short sizeCadena = 0;
			memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Se interpreta el valor variable de la clave.
			cadena = new char[sizeCadena + 1];
			memcpy(cadena, buffer, sizeCadena);
			buffer += sizeCadena;
			
			*(cadena + sizeCadena) = 0;			
			valor = cadena;
			
			delete[] cadena;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* IndiceVariableGriegoManager::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			char* cadena = NULL;
			unsigned int refRegistro = 0;		
			unsigned int hijoDer = 0;
			
			//Se interpreta la longitud del valor de la clave variable.
			unsigned short sizeCadena = 0;
			memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Se interpreta el valor variable de la clave.
			cadena = new char[sizeCadena + 1];
			memcpy(cadena, buffer, sizeCadena);
			buffer += sizeCadena;
			
			*(cadena + sizeCadena) = 0;			
			valor = cadena;
			
			delete[] cadena;
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
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
			//Copia de la longitud de la clave.
			unsigned short sizeCadena = clave->getTamanioValor();
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, clave->getValor(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		} 
			
		void IndiceVariableRomanoManager::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			
			//Copia de la longitud de la clave.
			unsigned short sizeCadena = clave->getTamanioValor();
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, clave->getValor(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			referencia = clave->getHijoDer();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		}	
	
		Clave* IndiceVariableRomanoManager::leerClaveHoja(char* &buffer)
		{
			string valor;
			char* cadena = NULL;
			unsigned int refRegistro = 0;
			
			//Se interpreta la longitud del valor de la clave variable.
			unsigned short sizeCadena = 0;
			memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Se interpreta el valor variable de la clave.
			cadena = new char[sizeCadena + 1];
			memcpy(cadena, buffer, sizeCadena);
			buffer += sizeCadena;
			
			*(cadena + sizeCadena) = 0;			
			valor = cadena;
			
			delete[] cadena;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* IndiceVariableRomanoManager::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			char* cadena = NULL;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta la longitud del valor de la clave variable.
			unsigned short sizeCadena = 0;
			memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
			buffer += Tamanios::TAMANIO_LONGITUD_CADENA;
			
			//Se interpreta el valor variable de la clave.
			cadena = new char[sizeCadena + 1];
			memcpy(cadena, buffer, sizeCadena);
			buffer += sizeCadena;
			
			*(cadena + sizeCadena) = 0;			
			valor = cadena;
			
			delete[] cadena;
			
			if (this->getTipoIndice() == TipoIndices::ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveVariable(valor, refRegistro, hijoDer);
		}

	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////

		SetClaves* IndiceVariableRomanoManager::leerListaClaves(unsigned int posicion)
		{		
			SetClaves* setClaves    = NULL;
			char* bufferClaves      = NULL; 
			unsigned int cantClaves = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
			
			//Parametros de inicializacion de la Capa Fisisca para
			//obtener una lista de claves primarias.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			pipe->agregarParametro(posicion, 2); //Numero de lista a eliminar dentro del archivo.
			
			//Iniciar comunicacion con la Capa Fisica
			pipe->lanzar();
			
			//Obtener tamaño de la lista
			pipe->leer(&cantClaves);
			
			if(cantClaves > 0) {
				bufferClaves = new char[this->getTamanioBloqueLista()];
				
				//Leer la lista del archivo.
				pipe->leer(this->getTamanioBloqueLista(), bufferClaves);
				
				char* strClave = NULL;
				unsigned int sizeClave = 0;
				string clave;
				
				setClaves = new SetClaves();
				for (unsigned char i = 0; i < cantClaves; i++){
					
					//Obtencion de la longitud de la clave variable.
					memcpy(&sizeClave, bufferClaves, Tamanios::TAMANIO_LONGITUD_CADENA);
					strClave = new char[sizeClave + 1];
					
					//Obtencion de la clave variable de longitud sizeClave.
					memcpy(strClave, bufferClaves, sizeClave);
					*(strClave + sizeClave) = 0;
					clave = strClave;
					
					//Insercion de la clave en el set.
					setClaves->insert(new ClaveVariable(clave));
					
					delete[] strClave;
				}
			}
			
			return setClaves;
		}
		
		unsigned int IndiceVariableRomanoManager::escribirListaClaves(SetClaves* setClaves)
		{		
			unsigned int numBloque = 0;
			char* bufferClaves     = NULL; 
			ComuDatos* pipe        = NULL;
			
			if (setClaves->size() > 0){
				//Instancia del pipe
				pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
				
				//Parametros de inicializacion de la Capa Fisisca para
				//obtener una lista de claves primarias.
				pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
				pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
				pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
			
				//Iniciar comunicacion con la Capa Fisica
				pipe->lanzar();
				
				bufferClaves = new char[this->getTamanioBloqueLista()];		
				unsigned int offset      = 0;
				unsigned short sizeClave = 0;
				
				//Contruccion del buffer con los datos de longitud variables contenidos 
				//en el set.
				for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it){
					//Obtencion de la longitud de la cadena.
					sizeClave = (short) (*it)->getTamanioValor();
					
					//Copia de la longitud y la cadena al buffer.
					memcpy(bufferClaves + offset, &sizeClave, Tamanios::TAMANIO_LONGITUD_CADENA);
					memcpy(bufferClaves + offset + Tamanios::TAMANIO_LONGITUD_CADENA, (*it)->getValor(), sizeClave);
					
					offset += (*it)->getTamanioValor() + Tamanios::TAMANIO_LONGITUD_CADENA;
				}
				
				//Escritura de la cantidad de claves en la lista
				pipe->escribir(setClaves->size());
				
				//Escritura de la lista al archivo.
				pipe->escribir(bufferClaves, this->getTamanioBloqueLista());
				
				//Obtencion del numero de bloque donde fue insertada la lista.
				pipe->leer(&numBloque);	
			}
			
			if (pipe)
				delete pipe;
			
			if (bufferClaves)
				delete bufferClaves;
			
			return numBloque;
		}
		
		char IndiceVariableRomanoManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves)
		{		
			char resultado     = 0;
			char* bufferClaves = NULL; 
			ComuDatos* pipe    = NULL;
			
			if (setClaves->size() > 0){
				//Instancia del pipe
				pipe = instanciarPipe("ernesto"/*NOMBRE_CAPA_FISICA*/);
				
				//Parametros de inicializacion de la Capa Fisisca para
				//obtener una lista de claves primarias.
				pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
				pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
				pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
				pipe->agregarParametro(posicion, 3); //Numero de bloque contenedor de la lista a modificar
				
				//Iniciar comunicacion con la Capa Fisica
				pipe->lanzar();
				
				bufferClaves             = new char[this->getTamanioBloqueLista()];		
				unsigned int offset      = 0;
				unsigned short sizeClave = 0;
				
				//Contruccion del buffer con los datos de longitud variables contenidos 
				//en el set.
				for (SetClaves::iterator it = setClaves->begin(); it != setClaves->end(); ++it){
					//Obtencion de la longitud de la cadena.
					sizeClave = (short) (*it)->getTamanioValor();
					
					//Copia de la longitud y la cadena al buffer.
					memcpy(bufferClaves + offset, &sizeClave, Tamanios::TAMANIO_LONGITUD_CADENA);
					memcpy(bufferClaves + offset + Tamanios::TAMANIO_LONGITUD_CADENA, (*it)->getValor(), sizeClave);
					
					offset += (*it)->getTamanioValor() + Tamanios::TAMANIO_LONGITUD_CADENA;
				}
				
				//Escritura de la cantidad de claves en la lista
				pipe->escribir(setClaves->size());
				
				//Escritura de la lista al archivo.
				pipe->escribir(bufferClaves, this->getTamanioBloqueLista());
				
				//Obtencion del resultado de la operacion
				pipe->leer(&resultado);
			}
			
			if (pipe)
				delete pipe;
			
			if (bufferClaves)
				delete bufferClaves;
			
			return resultado;
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
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
						
						delete (int*)valor;					
					}else if (tipo == TipoDatos::TIPO_BOOL){
						valor = new bool;
						tamanio = sizeof(bool);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
						
						delete (bool*)valor;
					}else if (tipo == TipoDatos::TIPO_CHAR){
						valor = new char;
						tamanio = sizeof(char);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveChar(*((char*)valor)));
						
						delete (char*)valor;						
					}else if (tipo == TipoDatos::TIPO_SHORT){
						valor = new short;
						tamanio = sizeof(short);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveShort(*((short*)valor)));
						
						delete (short*)valor;						
					}else if (tipo == TipoDatos::TIPO_FLOAT){
						valor = new float;
						tamanio = sizeof(float);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveReal(*((float*)valor)));
						
						delete (float*)valor;
					}else if (tipo == TipoDatos::TIPO_FECHA){
						valor = new ClaveFecha::TFECHA;
						tamanio = sizeof(ClaveFecha::TFECHA);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
						
						delete (ClaveFecha::TFECHA*)valor;
						
					}else if (tipo == TipoDatos::TIPO_VARIABLE){
						valor = new string;
						valor = buffer;
						tamanio = ((string*)valor)->size() + 1;
	
						listaClaves.push_back(new ClaveVariable(*((string*)valor)));
						delete (string*)valor;
					}
					
					buffer += tamanio;
				}
	
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
			buffer += Tamanios::TAMANIO_REFERENCIA;
						
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
					
					//Se interpreta el valor TFECHA de la clave.
					memcpy(&(((ClaveFecha::TFECHA*)valor)->anio), buffer, sizeof(unsigned short));
					buffer += sizeof(unsigned short);
					
					memcpy(&(((ClaveFecha::TFECHA*)valor)->mes), buffer, sizeof(unsigned char));
					buffer += sizeof(unsigned char);
					
					memcpy(&(((ClaveFecha::TFECHA*)valor)->dia), buffer, sizeof(unsigned char));
					buffer += sizeof(unsigned char);
					
					listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
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
			buffer += Tamanios::TAMANIO_REFERENCIA;
			
			//Crear la clave
			return new ClaveCompuesta(listaClaves, refRegistro);
		}


