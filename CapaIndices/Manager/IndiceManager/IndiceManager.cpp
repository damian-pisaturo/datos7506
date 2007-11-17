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
#include "../../Hash/Bucket.h"

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
							: Manager(tamanioBloque, nombreArchivo)
{
	switch (tipoIndice) {
	
		case TipoIndices::ARBOL_BP:
			this->setNombreArchivo(nombreArchivo + ".plus");
			break;
		case TipoIndices::ARBOL_BS:
			this->setNombreArchivo(nombreArchivo + ".star");
			break;
		case TipoIndices::HASH:
			this->setNombreArchivo(nombreArchivo + ".hash");
			break;
			
	}
	
	this->tipoIndice = tipoIndice;
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
IndiceArbolManager::IndiceArbolManager(unsigned int tamNodo,string nombreArchivo, unsigned char tipoIndice) :
	IndiceManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

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
	puntero += Tamanios::TAMANIO_REFERENCIA;
}

void IndiceArbolManager::copiarClaveNoHoja(Clave* clave, char* &puntero) 
{
	unsigned int referencia = 0;

	//Copia del valor de la clave
	memcpy(puntero, clave->getValor(), clave->getTamanioValor());
	puntero += clave->getTamanioValor();

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
		//Si el arbol es B*, copiar referencia al registro de datos.			
		referencia = clave->getReferencia();
		memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
		puntero += Tamanios::TAMANIO_REFERENCIA;
	}

	//Copia de la referencia al hijo derecho.
	referencia = clave->getHijoDer();
	memcpy(puntero, &referencia, Tamanios::TAMANIO_REFERENCIA);
	puntero += Tamanios::TAMANIO_REFERENCIA;
}

int IndiceArbolManager::leerBloque(unsigned int numBloque, BloqueIndice* bloqueLeido) 
{
	char resultado = 0;
	Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);

	//Buffer
	char* data = new char[this->getTamanioBloque() + 1];
	char* punteroAux = data;

	//Variables de interpretacion del nodo		
	Clave* claveNueva= NULL;
	SetClaves* set = new SetClaves();
	HeaderNodo headerNodo;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica para
	//leer un nodo de disco.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_NODO, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
	pipe->agregarParametro(numBloque, 3); //Numero de nodo a leer dentro del archivo

	//Se lanza el proceso de la capa fisica. 
	//Se obtiene en buffer el contenido del Nodo solicitado.
	pipe->lanzar();

	pipe->leer(&resultado);

	if (resultado == ResultadosFisica::OK) {
		pipe->leer(this->getTamanioBloque(), data);

		//Castear el header				
		memcpy(&headerNodo.nivel, data, sizeof(unsigned char));
		data += sizeof(unsigned char);

		//El hijo izq se carga siempre y cuando no se trate de un nodo hoja de un árbol B*
		if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
			memcpy(&headerNodo.refNodo, data, sizeof(unsigned int));
			data += sizeof(unsigned int);
		} else headerNodo.refNodo = 0;

		memcpy(&headerNodo.espacioLibre, data, sizeof(unsigned short));
		data += sizeof(unsigned short);

		//Settear el espacio libre, el nivel y la referencia del nodo
		nodoLeido->setNivel(headerNodo.nivel);
		nodoLeido->setEspacioLibre(headerNodo.espacioLibre);
		nodoLeido->setRefNodo(headerNodo.refNodo);

		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.
		const char* punteroFinal = punteroAux + (this->getTamanioBloque()
				- headerNodo.espacioLibre);

		if (nodoLeido->getNivel() == 0) {
			while (data < punteroFinal) {
				//Leer la clave	
				claveNueva = this->leerClaveHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}
		} else {
			while (data < punteroFinal) {
				//Leer la clave	
				claveNueva = this->leerClaveNoHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}
		}

		//Agregar el setClaves al nodo
		nodoLeido->setClaves(set);

		//Settear la posicion del nodo en el archivo
		nodoLeido->setPosicionEnArchivo(numBloque);
	}

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return resultado;
}

int IndiceArbolManager::escribirBloque(BloqueIndice* bloqueNuevo) 
{
	Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);

	//Variables de escritura del buffer
	char* data = new char[this->getTamanioBloque()];
	char* punteroAux = data;
	memset(data, 0, this->getTamanioBloque());

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	SetClaves* set= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

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
	
	//El hijo izq se guarda siempre y cuando no se trate de un nodo hoja de un árbol B*
	if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
		memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
		data+=sizeof(unsigned int);
	} else headerNodo.refNodo = 0;
	
	memcpy(data, &headerNodo.espacioLibre, sizeof(unsigned short));
	data+=sizeof(unsigned short);

	//Obtener la lista de claves
	set = nodoNuevo->getClaves();

	//Recorrer la lista de claves copiando cada clave al buffer
	if (nodoNuevo->getNivel() == 0) {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveHoja((Clave*)(*iterClaves), data);
	} else {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
	}

	//Grabar el buffer en el archivo.
	pipe->escribir(punteroAux, this->getTamanioBloque());

	//Obtener nueva posicion del bloque en el archivo. 
	unsigned int numBloque = 0;
	pipe->leer(&numBloque);

	//Setear en el nodo la posicion donde se grabo el nodo.
	nodoNuevo->setPosicionEnArchivo(numBloque);

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return numBloque;
}

int IndiceArbolManager::escribirBloque(unsigned short numNodo, BloqueIndice* bloqueModif) 
{
	char resultado = 0;
	Nodo* nodoNuevo = static_cast<Nodo*> (bloqueModif);

	//Variables de escritura del buffer
	char* data = new char[this->getTamanioBloque()];
	const char* punteroAux = data;
	memset(data, 0, this->getTamanioBloque());

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	SetClaves* set= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica.
	pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_NODO, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
	pipe->agregarParametro(numNodo, 3); //Numero de nodo a sobre-escribir.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Copiar el header al buffer
	headerNodo.nivel = nodoNuevo->getNivel();
	headerNodo.refNodo = nodoNuevo->getRefNodo();
	headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();

	memcpy(data, &headerNodo.nivel, sizeof(char));
	data+=sizeof(char);
	
	//El hijo izq se guarda siempre y cuando no se trate de un nodo hoja de un árbol B*
	if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
		memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
		data+=sizeof(unsigned int);
	} else headerNodo.refNodo = 0;
	
	memcpy(data, &headerNodo.espacioLibre, sizeof(unsigned short));
	data+=sizeof(unsigned short);

	//Obtener la lista de claves
	set = nodoNuevo->getClaves();

	//Recorrer la lista de claves copiando cada clave al buffer
	if (nodoNuevo->getNivel() == 0) {

		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveHoja((Clave*)(*iterClaves), data);

	} else {

		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveNoHoja((Clave*)(*iterClaves), data);

	}

	//Grabar el buffer en el archivo.
	pipe->escribir(punteroAux, this->getTamanioBloque());

	//Solicitar resultado de la comunicacion con la 
	//capa fisica.
	pipe->leer(&resultado);

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return resultado;
}

int IndiceArbolManager::eliminarBloque(unsigned short posicion) 
{
	char resultado = 0;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros para inicializar el pipe.
	pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_NODO, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
	pipe->agregarParametro(posicion, 3); //Posicion del bloque a eliminar.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Solicitar resultado de la comunicacion con la 
	//capa fisica.
	pipe->leer(&resultado);

	if (pipe)
		delete pipe;

	return resultado;
}

int IndiceArbolManager::escribirBloqueDoble(BloqueIndice* bloqueNuevo)
{
	unsigned int numBloque = 0;
	Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);

	//Variables de escritura del buffer
	char* data = new char[2*this->getTamanioBloque()];
	char* punteroAux = data;
	memset(data, 0, 2*this->getTamanioBloque());

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	SetClaves* set= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

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

	memcpy(data, &headerNodo.nivel, sizeof(char));
	data+=sizeof(char);
	
	//El hijo izq se guarda siempre y cuando no se trate de un nodo hoja de un árbol B*
	if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
		memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
		data+=sizeof(unsigned int);
	} else headerNodo.refNodo = 0;
	
	memcpy(data, &headerNodo.espacioLibre, sizeof(unsigned short));
	data+=sizeof(unsigned short);

	//Obtener la lista de claves
	set = nodoNuevo->getClaves();

	//Recorrer la lista de claves copiando cada clave al buffer
	if (nodoNuevo->getNivel() == 0) {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveHoja((Clave*)(*iterClaves), data);

	} else {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
	}

	//Grabar el primer bloque en el archivo.	
	pipe->escribir(punteroAux, this->getTamanioBloque());

	//Grabar el segundo bloque en el archivo.
	pipe->escribir(punteroAux + this->getTamanioBloque(),
			this->getTamanioBloque());

	//Obtener nueva posicion del primer nodo en el archivo
	//y settearla en el nodo. La segunda posicion es contigua.
	pipe->leer(&numBloque);
	nodoNuevo->setPosicionEnArchivo(numBloque);

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return numBloque;
}

int IndiceArbolManager::escribirBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueNuevo) 
{
	char resultado = 0;
	Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);

	//Variables de escritura del buffer
	char* data = new char[2*this->getTamanioBloque()];
	char* punteroAux = data;
	memset(data, 0, 2*this->getTamanioBloque());

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	SetClaves* set= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica.
	pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_NODO_DOBLE, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
	pipe->agregarParametro(numBloque, 3); // Numero del primer nodo a escribir a disco.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Copiar el header al buffer
	headerNodo.nivel = nodoNuevo->getNivel();
	headerNodo.refNodo = nodoNuevo->getRefNodo();
	headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();

	memcpy(data, &headerNodo.nivel, sizeof(char));
	data+=sizeof(char);
	
	//El hijo izq se guarda siempre y cuando no se trate de un nodo hoja de un árbol B*
	if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
		memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
		data+=sizeof(unsigned int);
	} else headerNodo.refNodo = 0;
	
	memcpy(data, &headerNodo.espacioLibre, sizeof(unsigned short));
	data+=sizeof(unsigned short);

	//Obtener la lista de claves
	set = nodoNuevo->getClaves();

	//Recorrer la lista de claves copiando cada clave al buffer
	if (nodoNuevo->getNivel() == 0) {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveHoja((Clave*)(*iterClaves), data);

	} else {
		for (SetClaves::iterator iterClaves = set->begin(); iterClaves
				!= set->end(); ++iterClaves)
			this->copiarClaveNoHoja((Clave*)(*iterClaves), data);
	}

	//Grabar el primer bloque en el archivo.	
	pipe->escribir(punteroAux, this->getTamanioBloque());

	//Grabar el segundo bloque en el archivo.
	pipe->escribir(punteroAux + this->getTamanioBloque(),
			this->getTamanioBloque());

	//Solicitar resultado de la comunicacion con la 
	//capa fisica.
	pipe->leer(&resultado);

	// Settear la posicion del bloque en disco (pasada por parametro).
	nodoNuevo->setPosicionEnArchivo(numBloque);

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return resultado;
}

int IndiceArbolManager::leerBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueLeido) 
{
	char resultado = 0;
	Nodo* nodoLeido = static_cast<Nodo*> (bloqueLeido);

	//Variables de escritura del buffer
	char* data = new char[this->getTamanioBloque()*2];
	char* punteroAux = data;

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	Clave* claveNueva= NULL;
	SetClaves* set = new SetClaves();

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica para
	//leer un nodo de disco.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_NODO_DOBLE, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
	pipe->agregarParametro(numBloque, 3); //Numero del primer nodo a leer dentro del archivo

	//Se lanza el proceso de la capa fisica.
	pipe->lanzar();

	//Se obtiene el resultado de la lectura 
	pipe->leer(&resultado);

	if (resultado == ResultadosFisica::OK) {
		//Se obtiene en buffer el contenido del primer nodo solicitado.
		pipe->leer(this->getTamanioBloque(), data);

		//Se obtiene en buffer el contenido del segundo nodo solicitado.
		pipe->leer(this->getTamanioBloque(), data + this->getTamanioBloque());

		//Castear el header				
		memcpy(&headerNodo.nivel, data, sizeof(unsigned char));
		data += sizeof(unsigned char);

		//El hijo izq se carga siempre y cuando no se trate de un nodo hoja de un árbol B*
		if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
			memcpy(&headerNodo.refNodo, data, sizeof(unsigned int));
			data += sizeof(unsigned int);
		} else headerNodo.refNodo = 0;

		memcpy(&headerNodo.espacioLibre, data, sizeof(unsigned short));
		data += sizeof(unsigned short);

		//Settear el espacio libre, el nivel y la referencia del nodo
		nodoLeido->setNivel(headerNodo.nivel);
		nodoLeido->setEspacioLibre(headerNodo.espacioLibre);
		nodoLeido->setRefNodo(headerNodo.refNodo);

		//Cálculo especial para determinar el tamaño del bloque doble en memoria
		unsigned short tamanioEnMemoria = 4*(this->getTamanioBloque() - Nodo::getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA)/3;
		if (nodoLeido->getNivel() == 0) tamanioEnMemoria += Nodo::getTamanioHeader() - Tamanios::TAMANIO_REFERENCIA;
		else tamanioEnMemoria += Nodo::getTamanioHeader();
		
		//Recorrer el buffer desde donde quedo hasta que supere
		//el espacio libre, interpretando clave por clave.
		const char* punteroFinal = punteroAux + (tamanioEnMemoria - headerNodo.espacioLibre);

		if (nodoLeido->getNivel() == 0) {
			while (data < punteroFinal) {
				//Leer la clave	
				claveNueva = this->leerClaveHoja(data);
				//Agregarla a la lista	
				set->insert(claveNueva);
			}
		} else {
			while (data < punteroFinal) {
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
	}

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return resultado;
}

int IndiceArbolManager::eliminarBloqueDoble(unsigned short posicion) 
{
	char resultado = 0;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros para inicializar el pipe.
	pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_NODO_DOBLE, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
	pipe->agregarParametro(posicion, 3); //Posicion del primer bloque a eliminar.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Solicitar resultado de la comunicacion con la 
	//capa fisica.
	pipe->leer(&resultado);

	return resultado;
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
IndiceHashManager::IndiceHashManager(unsigned int tamBucket, string nombreArchivo) :
	IndiceManager(tamBucket, nombreArchivo, TipoIndices::HASH) 
	{ }

IndiceHashManager::~IndiceHashManager() 
	{ }

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////
int IndiceHashManager::leerBloque(unsigned int numBucket, BloqueIndice* bloqueLeido) 
{
	char resultado = 0;
	Bucket* bucketLeido = static_cast<Bucket*> (bloqueLeido);
	char* buffer = new char[this->getTamanioBloque()];
	memset(buffer, 0, this->getTamanioBloque());

	//Variable de interpretacion del bucket
	HeaderBucket headerBucket;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de ejecucion de la Capa Fisica para leer
	//un bucket de disco.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_BUCKET, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket.
	pipe->agregarParametro(numBucket, 3); //Numero de bucket a leer.

	//Se lanza el proceso de la capa fisica. 
	//Se obtiene en buffer el contenido del Bucket solicitado.	
	pipe->lanzar();

	pipe->leer(&resultado);

	if (resultado == ResultadosFisica::OK) {
		pipe->leer(this->getTamanioBloque(), buffer);

		bucketLeido->setDatos(buffer);

		memcpy(&(headerBucket.espLibre), buffer, Tamanios::TAMANIO_ESPACIO_LIBRE);
		buffer += Tamanios::TAMANIO_ESPACIO_LIBRE;
		
		memcpy(&(headerBucket.cantRegs), buffer,Tamanios::TAMANIO_CANTIDAD_REGISTROS);
		buffer += Tamanios::TAMANIO_CANTIDAD_REGISTROS;
		
		memcpy(&(headerBucket.tamDispersion), buffer, Tamanios::TAMANIO_DISPERSION);

		bucketLeido->setTamDispersion(headerBucket.tamDispersion);
		bucketLeido->setEspacioLibre(headerBucket.espLibre);
		bucketLeido->setCantRegs(headerBucket.cantRegs);
		bucketLeido->setNroBloque(numBucket);

	}

	if (pipe)
		delete pipe;

	return resultado;
}

int IndiceHashManager::escribirBloque(BloqueIndice* nuevoBloque)
{
	Bucket* bucketLeido = static_cast<Bucket*> (nuevoBloque);

	//Variable de escritura del buffer
	char* data= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

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
	unsigned int numBucket = 0;
	pipe->leer(&numBucket);

	//Setear en el bucket la posicion donde se grabo.
	bucketLeido->setNroBloque(numBucket);

	if (pipe)
		delete pipe;

	return numBucket;
}

int IndiceHashManager::escribirBloque(unsigned short numBucket, BloqueIndice* bloqueModif) 
{
	char resultado = ResultadosFisica::OK;
	Bucket* bucketLeido = static_cast<Bucket*> (bloqueModif);

	//Variables de escritura del buffer
	char* buffer= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

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

	if (pipe)
		delete pipe;

	return resultado;
}

int IndiceHashManager::eliminarBloque(unsigned short numBucket) 
{
	char resultado = ResultadosFisica::OK;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//eliminar un bucket de disco.
	pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_BUCKET, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bucket en disco
	pipe->agregarParametro(numBucket, 3); //Numero de bucket a eliminar dentro del archivo.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Solicitar resultado de la comunicacion con la 
	//capa fisica.
	pipe->leer(&resultado);

	if (pipe)
		delete pipe;

	return resultado;
}

void IndiceHashManager::leerTabla(unsigned int* tamanio, unsigned int* &buckets) 
{
	char* bucketsTabla = NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica para
	//leer la tabla de dispersion 
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_TABLA_HASH, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.

	//Se lanza el proceso de la capa fisica. 
	pipe->lanzar();

	//Obtener tamaño de la tabla
	pipe->leer(tamanio);

	if (tamanio > 0) {
		unsigned int sizeTabla = (sizeof(unsigned int))*(*tamanio);

		bucketsTabla = new char[sizeTabla];

		//Obtener datos de la tabla				
		pipe->leer(sizeTabla, bucketsTabla);
	}

	buckets = (unsigned int*) bucketsTabla;

	if (pipe)
		delete pipe;
}

void IndiceHashManager::escribirTabla(unsigned int tamanio, unsigned int* buckets) 
{
	char* bucketsTabla= NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

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
	pipe->escribir(bucketsTabla, tamanio*sizeof(unsigned int));

	if (pipe)
		delete pipe;
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
IndiceSecundarioManager::IndiceSecundarioManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice)
{
	this->tamBloqueLista = tamBloqueLista;
}

IndiceSecundarioManager::~IndiceSecundarioManager() 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos publicos
//////////////////////////////////////////////////////////////////////
char IndiceSecundarioManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves) 
{
	char resultado     = 0;
	char* bufferClaves = NULL;
	ComuDatos* pipe    = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco
		pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset = 0;

		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			memcpy(bufferClaves + offset, (*it)->getValor(), (*it)->getTamanioValor());
			offset += (*it)->getTamanioValor();
		}

		//Escritura del tamanio de la lista (cantidad de claves a almacenar)
		pipe->escribir((int)setClaves->size());

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

int IndiceSecundarioManager::escribirListaClaves(SetClaves* setClaves)
{
	unsigned int numBloque = 0;
	char* bufferClaves     = NULL;
	ComuDatos* pipe        = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo de la lista en disco

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset = 0;

		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			memcpy(bufferClaves + offset, (*it)->getValor(), (*it)->getTamanioValor());
			offset += (*it)->getTamanioValor();
		}

		//Escritura de la cantidad de claves en la lista
		pipe->escribir((int)setClaves->size());

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

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceEnteroGriegoManager 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceEnteroGriegoManager::IndiceEnteroGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceEnteroGriegoManager::leerClaveHoja(char* &buffer)
{
	int valor                = 0;
	unsigned int refRegistro = 0;

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
	int valor                = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor entero de la clave.
	memcpy(&valor, buffer, sizeof(int));
	buffer+=sizeof(int);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
		//Si el arbol es B*, copiar referencia al registro de datos.
		memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		buffer += Tamanios::TAMANIO_REFERENCIA;
	}

	//Se interpreta la referencia al hijo derecho de la clave.
	memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

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
IndiceEnteroRomanoManager::IndiceEnteroRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
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
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++)
			setClaves->insert(new ClaveEntera( *((int*)bufferClaves) + i));
	}
	
	if (bufferClaves)
		delete[] bufferClaves;
	
	if (pipe)
		delete pipe;
	
	return setClaves;
}

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////		
Clave* IndiceEnteroRomanoManager::leerClaveHoja(char* &buffer) 
{
	int valor                = 0;
	unsigned int refRegistro = 0;

	//Se interpreta el valor entero de la clave.
	memcpy(&valor, buffer, sizeof(int));
	buffer += sizeof(int);

	//Se interpreta la referencia al registro de datos.
	memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveEntera(valor, refRegistro);
}

Clave* IndiceEnteroRomanoManager::leerClaveNoHoja(char* &buffer) 
{
	int valor                = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor entero de la clave.
	memcpy(&valor, buffer, sizeof(int));
	buffer += sizeof(int);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
IndiceBooleanRomanoManager::IndiceBooleanRomanoManager(unsigned int tamNodo, string nombreArchivo,
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos publicos
//////////////////////////////////////////////////////////////////////
SetClaves* IndiceBooleanRomanoManager::leerListaClaves(unsigned int posicion) 
{
	SetClaves* setClaves    = NULL;
	char* bufferClaves      = NULL;
	unsigned int cantClaves = 0;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++)
			setClaves->insert(new ClaveBoolean( *((bool*)bufferClaves) + i));
	}
	
	if (bufferClaves)
		delete[] bufferClaves;
	
	if (pipe)
		delete pipe;

	return setClaves;
}

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////			
Clave* IndiceBooleanRomanoManager::leerClaveHoja(char* &buffer) 
{
	bool valor               = false;
	unsigned int refRegistro = 0;

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
	bool valor               = false;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor booleano de la clave.
	memcpy(&valor, buffer, sizeof(bool));
	buffer += sizeof(bool);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
		//Si el arbol es B*, copiar referencia al registro de datos.
		memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		buffer += Tamanios::TAMANIO_REFERENCIA;
	}

	//Se interpreta la referencia al hijo derecho de la clave.
	memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

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
IndiceCharGriegoManager::IndiceCharGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////		
Clave* IndiceCharGriegoManager::leerClaveHoja(char* &buffer)
{
	char valor               = 0;
	unsigned int refRegistro = 0;

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
	char valor               = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor de tipo char de la clave.
	memcpy(&valor, buffer, sizeof(char));
	buffer += sizeof(char);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
		//Si el arbol es B*, copiar referencia al registro de datos.
		memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		buffer += Tamanios::TAMANIO_REFERENCIA;
	}

	//Se interpreta la referencia al hijo derecho de la clave.
	memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

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
IndiceCharRomanoManager::IndiceCharRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceCharRomanoManager::leerClaveHoja(char* &buffer)
{
	char valor               = 0;
	unsigned int refRegistro = 0;

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
	char valor               = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor de tipo char de la clave.
	memcpy(&valor, buffer, sizeof(char));
	buffer += sizeof(char);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
		//Si el arbol es B*, copiar referencia al registro de datos.
		memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		buffer += Tamanios::TAMANIO_REFERENCIA;
	}

	//Se interpreta la referencia al hijo derecho de la clave.
	memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveChar(valor, refRegistro, hijoDer);
}

SetClaves* IndiceCharRomanoManager::leerListaClaves(unsigned int posicion) 
{
	SetClaves* setClaves    = NULL;
	char* bufferClaves      = NULL;
	unsigned int cantClaves = 0;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++)
			setClaves->insert(new ClaveChar(*((char*)bufferClaves) + i));
	}
	
	if (bufferClaves)
		delete[] bufferClaves;
	
	if (pipe)
		delete pipe;

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
IndiceShortGriegoManager::IndiceShortGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceShortGriegoManager::leerClaveHoja(char* &buffer) 
{
	short valor              = 0;
	unsigned int refRegistro = 0;

	//Se interpreta el valor short de la clave.
	memcpy(&valor, buffer, sizeof(short));
	buffer += sizeof(short);

	//Se interpreta la referencia al registro de datos.
	memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveShort(valor, refRegistro);
}

Clave* IndiceShortGriegoManager::leerClaveNoHoja(char* &buffer) 
{
	short valor              = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor short de la clave.
	memcpy(&valor, buffer, sizeof(short));
	buffer += sizeof(short);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
IndiceShortRomanoManager::IndiceShortRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceShortRomanoManager::leerClaveHoja(char* &buffer)
{
	short valor              = 0;
	unsigned int refRegistro = 0;

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
	short valor              = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor short de la clave.
	memcpy(&valor, buffer, sizeof(short));
	buffer += sizeof(short);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++)
			setClaves->insert(new ClaveShort(*((short*)bufferClaves) + i));
	}
	
	if (bufferClaves)
		delete[] bufferClaves;
	
	if (pipe)
		delete pipe;
	
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
IndiceRealRomanoManager::IndiceRealRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceRealRomanoManager::leerClaveHoja(char* &buffer) 
{
	float valor              = 0;
	unsigned int refRegistro = 0;

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
	float valor              = 0;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor float de la clave.
	memcpy(&valor, buffer, sizeof(float));
	buffer += sizeof(float);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++)
			setClaves->insert(new ClaveReal(*((float*)bufferClaves) + i));
	}

	if (bufferClaves)
		delete[] bufferClaves;
	
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
IndiceFechaGriegoManager::IndiceFechaGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceFechaGriegoManager::leerClaveHoja(char* &buffer) 
{
	ClaveFecha::TFECHA valor;
	unsigned int refRegistro = 0;

	//Se interpreta el valor TFECHA de la clave.
	memcpy(&(valor.anio), buffer, sizeof(unsigned short));
	buffer += sizeof(short);

	memcpy(&(valor.mes), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	memcpy(&(valor.dia), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	//Se interpreta la referencia al registro de datos.
	memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveFecha(valor, refRegistro);
}

Clave* IndiceFechaGriegoManager::leerClaveNoHoja(char* &buffer) 
{
	ClaveFecha::TFECHA valor;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor TFECHA de la clave.
	memcpy(&(valor.anio), buffer, sizeof(unsigned short));
	buffer += sizeof(unsigned short);

	memcpy(&(valor.mes), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	memcpy(&(valor.dia), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
IndiceFechaRomanoManager::IndiceFechaRomanoManager(unsigned int tamNodo, const string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos publicos
//////////////////////////////////////////////////////////////////////	
SetClaves* IndiceFechaRomanoManager::leerListaClaves(unsigned int posicion)
{
	SetClaves* setClaves     = NULL;
	char* bufferClaves       = NULL;
	unsigned int cantClaves  = 0;
	ClaveFecha::TFECHA fecha;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		int offset = 0;

		setClaves = new SetClaves();
		
		for (unsigned char i = 0; i < cantClaves; i++){
			memcpy(&fecha.anio, bufferClaves + offset, sizeof(unsigned short));
			offset += sizeof(unsigned short);
			memcpy(&fecha.mes, bufferClaves + offset, sizeof(unsigned char));
			offset += sizeof(unsigned char);
			memcpy(&fecha.dia, bufferClaves + offset, sizeof(unsigned char));
			offset += sizeof(unsigned char);

			setClaves->insert(new ClaveFecha(fecha));
		}
	}
	
	if (bufferClaves)
		delete[] bufferClaves;
	
	if (pipe)
		delete pipe;

	return setClaves;
}

char IndiceFechaRomanoManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves) 
{
	char resultado            = 0;
	char* bufferClaves        = NULL;
	ComuDatos* pipe           = NULL;
	ClaveFecha::TFECHA* fecha = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
		pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves        = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset = 0;

		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			fecha = (ClaveFecha::TFECHA*)(*it)->getValor();
			memcpy(bufferClaves + offset, &fecha->anio, sizeof(unsigned short));
			memcpy(bufferClaves + offset + sizeof(unsigned short), &fecha->mes,
					sizeof(unsigned char));
			memcpy(bufferClaves + offset + sizeof(unsigned short)
					+ sizeof(unsigned char), &fecha->dia, sizeof(unsigned char));
			offset += (*it)->getTamanioValor();
		}

		//Escritura del tamanio de la lista (cantidad de claves a almacenar)
		pipe->escribir((int)setClaves->size());

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

int IndiceFechaRomanoManager::escribirListaClaves(SetClaves* setClaves) 
{
	int numBloque             = 0;
	char* bufferClaves        = NULL;
	ComuDatos* pipe           = NULL;
	ClaveFecha::TFECHA* fecha = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves        = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset = 0;

		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			fecha = (ClaveFecha::TFECHA*)(*it)->getValor();
			memcpy(bufferClaves + offset, &fecha->anio, sizeof(unsigned short));
			memcpy(bufferClaves + offset + sizeof(unsigned short), &fecha->mes,
					sizeof(unsigned char));
			memcpy(bufferClaves + offset + sizeof(unsigned short)
					+ sizeof(unsigned char), &fecha->dia, sizeof(unsigned char));
			offset += (*it)->getTamanioValor();
		}

		//Escritura del tamanio de la lista (cantidad de claves a almacenar)
		pipe->escribir((int)setClaves->size());

		//Escritura de la lista al archivo.
		pipe->escribir(bufferClaves, this->getTamanioBloqueLista());

		//Obtencion del resultado de la operacion
		pipe->leer(&numBloque);
	}

	if (pipe)
		delete pipe;

	if (bufferClaves)
		delete[] bufferClaves;

	return numBloque;
}

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////
Clave* IndiceFechaRomanoManager::leerClaveHoja(char* &buffer) 
{
	ClaveFecha::TFECHA valor;
	unsigned int refRegistro = 0;

	//Se interpreta el valor TFECHA de la clave.
	memcpy(&(valor.anio), buffer, sizeof(unsigned short));
	buffer += sizeof(unsigned short);

	memcpy(&(valor.mes), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	memcpy(&(valor.dia), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	//Se interpreta la referencia al registro de datos.
	memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveFecha(valor, refRegistro);
}

Clave* IndiceFechaRomanoManager::leerClaveNoHoja(char* &buffer) 
{
	ClaveFecha::TFECHA valor;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;

	//Se interpreta el valor TFECHA de la clave.
	memcpy(&(valor.anio), buffer, sizeof(unsigned short));
	buffer += sizeof(unsigned short);

	memcpy(&(valor.mes), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	memcpy(&(valor.dia), buffer, sizeof(unsigned char));
	buffer += sizeof(unsigned char);

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
IndiceVariableGriegoManager::IndiceVariableGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	

void IndiceVariableGriegoManager::copiarClaveHoja(Clave* clave, char* &buffer) 
{
	unsigned char sizeCadena = clave->getTamanioValor();
	
	//Copia de la longitud de la clave.	
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
	unsigned char sizeCadena = clave->getTamanioValor();
	
	//Copia de la longitud de la clave.	
	memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Copia del valor variable (char*) de la clave
	memcpy(buffer, clave->getValor(), sizeCadena);
	buffer += sizeCadena;

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	char* cadena             = NULL;
	unsigned int refRegistro = 0;
	unsigned char sizeCadena = 0;
	
	//Se interpreta la longitud del valor de la clave variable.
	memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Se interpreta el valor variable de la clave.
	cadena = new char[sizeCadena + 1];
	memcpy(cadena, buffer, sizeCadena);
	buffer += sizeCadena;

	*(cadena + sizeCadena) = 0;
	valor = cadena;

	if (cadena)
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
	char* cadena             = NULL;
	unsigned int refRegistro = 0;
	unsigned int hijoDer     = 0;
	unsigned char sizeCadena = 0;

	//Se interpreta la longitud del valor de la clave variable.
	memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Se interpreta el valor variable de la clave.
	cadena = new char[sizeCadena + 1];
	memcpy(cadena, buffer, sizeCadena);
	buffer += sizeCadena;

	*(cadena + sizeCadena) = 0;
	valor = cadena;

	if (cadena)
		delete[] cadena;

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
IndiceVariableRomanoManager::IndiceVariableRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////
void IndiceVariableRomanoManager::copiarClaveHoja(Clave* clave, char* &buffer) 
{
	unsigned char sizeCadena = clave->getTamanioValor();
	
	//Copia de la longitud de la clave.	
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
	unsigned char sizeCadena = (unsigned char) clave->getTamanioValor();

	//Copia de la longitud de la clave.	
	memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Copia del valor variable (char*) de la clave
	memcpy(buffer, clave->getValor(), clave->getTamanioValor());
	buffer += clave->getTamanioValor();

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	char* cadena             = NULL;
	unsigned int refRegistro = 0;
	unsigned char sizeCadena = 0;
	
	//Se interpreta la longitud del valor de la clave variable.
	memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Se interpreta el valor variable de la clave.
	cadena = new char[sizeCadena + 1];
	memcpy(cadena, buffer, sizeCadena);
	buffer += sizeCadena;

	*(cadena + sizeCadena) = 0;
	valor = cadena;

	if (cadena)
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
	char* cadena              = NULL;
	unsigned int refRegistro  = 0;
	unsigned int hijoDer      = 0;
	unsigned char sizeCadena  = 0;
	
	//Se interpreta la longitud del valor de la clave variable.	
	memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
	buffer += Tamanios::TAMANIO_LONGITUD_CADENA;

	//Se interpreta el valor variable de la clave.
	cadena = new char[sizeCadena + 1];
	memcpy(cadena, buffer, sizeCadena);
	buffer += sizeCadena;

	*(cadena + sizeCadena) = 0;
	valor = cadena;

	if (cadena)
		delete[] cadena;

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);

		char* strClave          = NULL;
		unsigned char sizeClave = 0;
		string clave;

		setClaves = new SetClaves();
		for (unsigned char i = 0; i < cantClaves; i++) {

			//Obtencion de la longitud de la clave variable.
			memcpy(&sizeClave, bufferClaves, Tamanios::TAMANIO_LONGITUD_CADENA);
			strClave = new char[sizeClave + 1];

			//Obtencion de la clave variable de longitud sizeClave.
			memcpy(strClave, bufferClaves, sizeClave);
			*(strClave + sizeClave) = 0;
			clave = strClave;

			//Insercion de la clave en el set.
			setClaves->insert(new ClaveVariable(clave));

			if (strClave)
				delete[] strClave;
		}
	}
	
	if (bufferClaves)
		delete[] bufferClaves;

	return setClaves;
}

int IndiceVariableRomanoManager::escribirListaClaves(SetClaves* setClaves)
{
	unsigned int numBloque = 0;
	char* bufferClaves     = NULL;
	ComuDatos* pipe        = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves            = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset     = 0;
		unsigned char sizeClave = 0;

		//Contruccion del buffer con los datos de longitud variables contenidos 
		//en el set.
		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			//Obtencion de la longitud de la cadena.
			sizeClave = (unsigned char) (*it)->getTamanioValor();

			//Copia de la longitud y la cadena al buffer.
			memcpy(bufferClaves + offset, &sizeClave,
					Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(bufferClaves + offset + Tamanios::TAMANIO_LONGITUD_CADENA, (*it)->getValor(), sizeClave);

			offset += (*it)->getTamanioValor() + Tamanios::TAMANIO_LONGITUD_CADENA;
		}

		//Escritura de la cantidad de claves en la lista
		pipe->escribir((int)setClaves->size());

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

char IndiceVariableRomanoManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves) 
{
	char resultado     = 0;
	char* bufferClaves = NULL;
	ComuDatos* pipe    = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
		pipe->agregarParametro(posicion, 3); //Numero de bloque contenedor de la lista a modificar

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves            = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		unsigned int offset     = 0;
		unsigned char sizeClave = 0;

		//Contruccion del buffer con los datos de longitud variables contenidos 
		//en el set.
		for (SetClaves::iterator it = setClaves->begin(); it
				!= setClaves->end(); ++it) {
			//Obtencion de la longitud de la cadena.
			sizeClave = (unsigned char) (*it)->getTamanioValor();

			//Copia de la longitud y la cadena al buffer.
			memcpy(bufferClaves + offset, &sizeClave, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(bufferClaves + offset + Tamanios::TAMANIO_LONGITUD_CADENA, (*it)->getValor(), sizeClave);

			offset += sizeClave + Tamanios::TAMANIO_LONGITUD_CADENA;
		}

		//Escritura de la cantidad de claves en la lista
		pipe->escribir((int)setClaves->size());

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

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCompuestoGriegoManager 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceCompuestoGriegoManager::IndiceCompuestoGriegoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice, ListaTipos* listaTipos) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
{ 
	this->tipos = listaTipos;
}

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////

void IndiceCompuestoGriegoManager::copiarClaveHoja(Clave* clave, char* &buffer) 
{
	int tamanio                    = 0;
	ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
	ListaClaves* listaClaves       = claveCompuesta->getListaClaves();
	Clave* claveAux                = NULL;

	ListaTipos::const_iterator iterTipos; //Iterador de la lista de tipos de claves simples.
	
	//Variables de interpretacion de una ClaveVariable
	char* cadena             = NULL;
	unsigned char sizeCadena = 0;
	
	iterTipos = this->tipos->begin();
	
	//Copia de los valores de las claves en el nodo
	for (ListaClaves::const_iterator iterClaves = listaClaves->begin(); iterClaves != listaClaves->end(); ++iterClaves) {		
		claveAux = (*iterClaves);
		
		if (*iterTipos == TipoDatos::TIPO_VARIABLE){
			cadena = (char*)claveAux->getValor();
			sizeCadena = claveAux->getTamanioValor();
			
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(buffer + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
			
			tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
		}else{
			
			//Copia del valor de la clave
			memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
			tamanio = claveAux->getTamanioValor();
		}
		
		buffer += tamanio;
		++iterTipos;
	}

	//Copia de la referencia a registro del archivo de datos. 
	unsigned int referencia = clave->getReferencia();
	memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;
}

void IndiceCompuestoGriegoManager::copiarClaveNoHoja(Clave* clave, char* &buffer) 
{
	int tamanio                    = 0;
	unsigned int referencia        = 0;
	ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
	ListaClaves* listaClaves       = claveCompuesta->getListaClaves();
	Clave* claveAux                = NULL;

	ListaTipos::const_iterator iterTipos; //Iterador de la lista de tipos de claves simples.
	
	//Variables de interpretacion de una ClaveVariable
	char* cadena             = NULL;
	unsigned char sizeCadena = 0;
	
	iterTipos = this->tipos->begin();
	
	//Copia de los valores de las claves en el nodo
	for (ListaClaves::const_iterator iterClaves = listaClaves->begin(); iterClaves != listaClaves->end(); ++iterClaves) {		
		claveAux = (*iterClaves);
		
		if (*iterTipos == TipoDatos::TIPO_VARIABLE){
			cadena = (char*)claveAux->getValor();
			sizeCadena = claveAux->getTamanioValor();
			
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(buffer + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
			
			tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
		}else{
			
			//Copia del valor de la clave
			memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
			tamanio = claveAux->getTamanioValor();
		}
		
		buffer += tamanio;
		++iterTipos;
	}
	

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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
	void* valor= NULL;
	
	//Variables de interpretacion de claves
	//de longitud variable.
	char sizeCadena = 0;
	char* cadena = NULL;
	
	//Copia de los valores de las claves en el nodo				
	for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos
			!= this->tipos->end(); ++iterTipos) {

		tipo = (*iterTipos);

		if (tipo == TipoDatos::TIPO_ENTERO) {
			//Copia del valor de la clave
			valor = new int;
			tamanio = sizeof(int);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveEntera(*((int*)valor)));

			delete (int*)valor;
		} else if (tipo == TipoDatos::TIPO_BOOL) {
			valor = new bool;
			tamanio = sizeof(bool);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));

			delete (bool*)valor;
		} else if (tipo == TipoDatos::TIPO_CHAR) {
			valor = new char;
			tamanio = sizeof(char);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveChar(*((char*)valor)));

			delete (char*)valor;
		} else if (tipo == TipoDatos::TIPO_SHORT) {
			valor = new short;
			tamanio = sizeof(short);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveShort(*((short*)valor)));

			delete (short*)valor;
		} else if (tipo == TipoDatos::TIPO_FLOAT) {
			valor = new float;
			tamanio = sizeof(float);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveReal(*((float*)valor)));

			delete (float*)valor;
		} else if (tipo == TipoDatos::TIPO_FECHA) {
			valor = new ClaveFecha::TFECHA;
			tamanio = Tamanios::TAMANIO_FECHA;

			memcpy(& ((ClaveFecha::TFECHA*)valor)->anio, buffer, sizeof(unsigned short));
			memcpy(&((ClaveFecha::TFECHA*)valor)->mes, buffer + sizeof(unsigned short), sizeof(unsigned char));
			memcpy(&((ClaveFecha::TFECHA*)valor)->dia, buffer + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
			listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));

			delete (ClaveFecha::TFECHA*)valor;

		} else if (tipo == TipoDatos::TIPO_VARIABLE) {
			valor = new string;
			
			memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
			
			cadena = new char[sizeCadena*sizeof(char) + 1];		
			
			memcpy(cadena, buffer + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena*sizeof(char));
			*(cadena + sizeCadena) = 0;
			
			valor = cadena;
			
			tamanio = ((string*)valor)->size() + Tamanios::TAMANIO_LONGITUD_CADENA;

			listaClaves.push_back(new ClaveVariable(*((string*)valor)));
			
			if (valor)
				delete (string*)valor;
			
			if (cadena) 
				delete[] cadena;
						
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
	int tipo    = 0;
	unsigned int refRegistro = 0, hijoDer = 0, tamanio = 0;
	void* valor = NULL;

	//Copia de los valores de las claves en el nodo				
	for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos
			!= this->tipos->end(); ++iterTipos) {

		tipo = (*iterTipos);

		if (tipo == TipoDatos::TIPO_ENTERO) {
			//Copia del valor de la clave
			valor = new int;
			tamanio = sizeof(int);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveEntera(*((int*)valor)));

		} else if (tipo == TipoDatos::TIPO_BOOL) {
			valor = new bool;
			tamanio = sizeof(bool);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
		} else if (tipo == TipoDatos::TIPO_CHAR) {
			valor = new char;
			tamanio = sizeof(char);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveChar(*((char*)valor)));

		} else if (tipo == TipoDatos::TIPO_SHORT) {
			valor = new short;
			tamanio = sizeof(short);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveShort(*((short*)valor)));

		} else if (tipo == TipoDatos::TIPO_FLOAT) {
			valor = new float;
			tamanio = sizeof(float);

			memcpy(valor, buffer, tamanio);
			listaClaves.push_back(new ClaveReal(*((float*)valor)));
		} else if (tipo == TipoDatos::TIPO_FECHA) {
			valor = new ClaveFecha::TFECHA;
			tamanio = sizeof(ClaveFecha::TFECHA);

			//Se interpreta el valor TFECHA de la clave.
			memcpy(&(((ClaveFecha::TFECHA*)valor)->anio), buffer,
					sizeof(unsigned short));
			buffer += sizeof(unsigned short);

			memcpy(&(((ClaveFecha::TFECHA*)valor)->mes), buffer,
					sizeof(unsigned char));
			buffer += sizeof(unsigned char);

			memcpy(&(((ClaveFecha::TFECHA*)valor)->dia), buffer,
					sizeof(unsigned char));
			buffer += sizeof(unsigned char);

			listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
		} else if (tipo == TipoDatos::TIPO_VARIABLE) {
			valor = new string;
			valor = buffer;
			tamanio = ((string*)valor)->size() + 1;

			listaClaves.push_back(new ClaveVariable(*((string*)valor)));
		}

		buffer += tamanio;
	}

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCompuestoRomanoManager 
//		   (Implementa archivo de indices secundarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceCompuestoRomanoManager::IndiceCompuestoRomanoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned int tamBloqueLista, unsigned char tipoIndice,  ListaTipos* listaTipos) :
	IndiceSecundarioManager(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////

void IndiceCompuestoRomanoManager::copiarClaveHoja(Clave* clave, char* &buffer)
{
	ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
	ListaClaves* listaClaves       = claveCompuesta->getListaClaves();
	Clave* claveAux                = NULL;
	ListaTipos::const_iterator iterTipos; //Iterador de la lista de tipos de claves simples.
	unsigned int referencia        = 0;  //Rerefencia a registro de datos contenida en la clave.
	unsigned int tamanio           = 0;
	
	//Variables de interpretacion de una ClaveVariable
	char* cadena             = NULL;
	unsigned char sizeCadena = 0;
	
	iterTipos = this->tipos->begin();
	
	//Copia de los valores de las claves en el nodo
	for (ListaClaves::const_iterator iterClaves = listaClaves->begin(); iterClaves != listaClaves->end(); ++iterClaves) {		
		claveAux = (*iterClaves);
		
		if (*iterTipos == TipoDatos::TIPO_VARIABLE){
			cadena = (char*)claveAux->getValor();
			sizeCadena = claveAux->getTamanioValor();
			
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(buffer + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
			
			tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
			
		}else{
			
			//Copia del valor de la clave
			memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
			tamanio = claveAux->getTamanioValor();
		}
		
		buffer += tamanio;
		++iterTipos;
	}
	
	//Copia de la referencia a registro del archivo de datos. 
	referencia = clave->getReferencia();
	memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;
}

void IndiceCompuestoRomanoManager::copiarClaveNoHoja(Clave* clave, char* &buffer) 
{
	unsigned int referencia        = 0;
	ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
	ListaClaves* listaClaves       = claveCompuesta->getListaClaves();
	Clave* claveAux                = NULL;
	unsigned int tamanio           = 0;
	ListaTipos::const_iterator iterTipos; //Iterador de la lista de tipos de claves simples.
	
	//Variables de interpretacion de una ClaveVariable
	char* cadena             = NULL;
	unsigned char sizeCadena = 0;
	
	iterTipos = this->tipos->begin();
	
	//Copia de los valores de las claves en el nodo
	for (ListaClaves::const_iterator iterClaves = listaClaves->begin(); iterClaves != listaClaves->end(); ++iterClaves) {		
		claveAux = (*iterClaves);
		
		if (*iterTipos == TipoDatos::TIPO_VARIABLE){
			cadena = (char*)claveAux->getValor();
			sizeCadena = claveAux->getTamanioValor();
			
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(buffer + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
			
			tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
		}else{
			
			//Copia del valor de la clave
			memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
			tamanio = claveAux->getTamanioValor();
		}
		
		buffer += tamanio;
		++iterTipos;
	}

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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

Clave* IndiceCompuestoRomanoManager::leerClaveHoja(char* &buffer)
{
	ListaClaves listaClaves;
	int tipo = 0;
	unsigned int refRegistro = 0, tamanio = 0;
	void* valor = NULL;
	
	//Variables de interpretacion de claves de longitud variable
	unsigned char sizeCadena = 0;
	char* cadena             = NULL;

	//Copia de los valores de las claves en el nodo				
	for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos != this->tipos->end(); ++iterTipos) {

		tipo = (*iterTipos);

		switch(tipo){
			case TipoDatos::TIPO_ENTERO:
			{
				//Copia del valor entero de la clave
				valor = new int;
				tamanio = sizeof(int);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveEntera(*((int*)valor)));
	
				delete (int*)valor;
	
			}break;
			
			case TipoDatos::TIPO_BOOL:
			{
				//Copia del valor booleano de la clave
				valor = new bool;
				tamanio = sizeof(bool);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
	
				delete (bool*)valor;
				
			}break;
			
			case TipoDatos::TIPO_CHAR:
			{
				//Copia del valor tipo char de la clave.
				valor = new char;
				tamanio = sizeof(char);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveChar(*((char*)valor)));
	
				delete (char*)valor;
			
			}break;
			
			case TipoDatos::TIPO_SHORT:
			{
				//Copia del valor short de la clave
				valor = new short;
				tamanio = sizeof(short);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveShort(*((short*)valor)));
	
				delete (short*)valor;
				
			}break;
			
			case TipoDatos::TIPO_FLOAT:
			{
				//Copia del valor float de la clave
				valor = new float;
				tamanio = sizeof(float);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveReal(*((float*)valor)));
	
				delete (float*)valor;	
				
			}break;
			
			case TipoDatos::TIPO_FECHA:
			{
				//Copia del valor tipo TFECHA de la clave.
				valor = new ClaveFecha::TFECHA;
				tamanio = Tamanios::TAMANIO_FECHA;
	
				memcpy(&(((ClaveFecha::TFECHA*)valor)->anio), buffer, sizeof(unsigned short));
				memcpy(&(((ClaveFecha::TFECHA*)valor)->mes), buffer + sizeof(unsigned short), sizeof(unsigned char));
				memcpy(&(((ClaveFecha::TFECHA*)valor)->dia), buffer + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
				
				listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
	
				delete (ClaveFecha::TFECHA*)valor;
				
			}break;
			
			case TipoDatos::TIPO_VARIABLE:
			{
				valor = new string;
			
				memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
				
				cadena = new char[sizeCadena + 1];
				memcpy(cadena, buffer + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena);
				
				*(cadena + sizeCadena) = 0;			
				valor = cadena;
				
				listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				
				delete (string*)valor;	
				
			}break;
			
		} 

		buffer += tamanio;
	}

	//Se interpreta la referencia al registro de datos.
	memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
	buffer += Tamanios::TAMANIO_REFERENCIA;

	//Crear la clave
	return new ClaveCompuesta(listaClaves, refRegistro);
}

Clave* IndiceCompuestoRomanoManager::leerClaveNoHoja(char* &buffer) 
{
	ListaClaves listaClaves;
	int tipo = 0;
	unsigned int refRegistro = 0, hijoDer = 0, tamanio = 0;
	void* valor = NULL;

	//Variables de interpretacion de claves de longitud variable
	unsigned char sizeCadena = 0;
	char* cadena             = NULL;

	//Copia de los valores de las claves en el nodo				
	for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos != this->tipos->end(); ++iterTipos) {

		tipo = (*iterTipos);

		switch(tipo){
			case TipoDatos::TIPO_ENTERO:
			{
				//Copia del valor entero de la clave
				valor = new int;
				tamanio = sizeof(int);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveEntera(*((int*)valor)));
	
				delete (int*)valor;
	
			}break;
			
			case TipoDatos::TIPO_BOOL:
			{
				//Copia del valor booleano de la clave
				valor = new bool;
				tamanio = sizeof(bool);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
	
				delete (bool*)valor;
				
			}break;
			
			case TipoDatos::TIPO_CHAR:
			{
				//Copia del valor tipo char de la clave.
				valor = new char;
				tamanio = sizeof(char);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveChar(*((char*)valor)));
	
				delete (char*)valor;
			
			}break;
			
			case TipoDatos::TIPO_SHORT:
			{
				//Copia del valor short de la clave
				valor = new short;
				tamanio = sizeof(short);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveShort(*((short*)valor)));
	
				delete (short*)valor;
				
			}break;
			
			case TipoDatos::TIPO_FLOAT:
			{
				//Copia del valor float de la clave
				valor = new float;
				tamanio = sizeof(float);
	
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveReal(*((float*)valor)));
	
				delete (float*)valor;	
				
			}break;
			
			case TipoDatos::TIPO_FECHA:
			{
				//Copia del valor tipo TFECHA de la clave.
				valor = new ClaveFecha::TFECHA;
				tamanio = Tamanios::TAMANIO_FECHA;
	
				memcpy(&(((ClaveFecha::TFECHA*)valor)->anio), buffer, sizeof(unsigned short));
				memcpy(&(((ClaveFecha::TFECHA*)valor)->mes), buffer + sizeof(unsigned short), sizeof(unsigned char));
				memcpy(&(((ClaveFecha::TFECHA*)valor)->dia), buffer + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
				
				listaClaves.push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
	
				delete (ClaveFecha::TFECHA*)valor;
				
			}break;
			
			case TipoDatos::TIPO_VARIABLE:
			{
				valor = new string;
			
				memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
				
				cadena = new char[sizeCadena + 1];
				memcpy(cadena, buffer + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena);
				
				*(cadena + sizeCadena) = 0;			
				valor = cadena;
				
				listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				tamanio = Tamanios::TAMANIO_LONGITUD_CADENA + sizeCadena;
				
				delete (string*)valor;	
				
			}break;
			
		}
		
		buffer += tamanio;
	}

	if (this->getTipoIndice() == TipoIndices::ARBOL_BS) {
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

//////////////////////////////////////////////////////////////////////
// Metodos publicos
//////////////////////////////////////////////////////////////////////

SetClaves* IndiceCompuestoRomanoManager::leerListaClaves(unsigned int posicion) 
{
	ListaClaves* listaClaves = NULL; //Lista de claves contenidas en una clave compuesta
	SetClaves* setClaves     = NULL; //Set de claves compuestas
	char* bufferClaves       = NULL; //Buffer contenedor de toda la lista de claves en disco.
	void* valor              = NULL; //Valor de cada clave simple.
	unsigned int cantClaves  = 0;	 //Cantidad de claves en el buffer.
	int tipo                 = 0;    //Tipo de cada clave en el buffer.
	unsigned int tamanio     = 0;    //Tamanio de cada clave.
	
	//Variables auxiliares para la interpretacion de una clave 
	//de longitud variable.
	char* cadena              = NULL;
	unsigned short sizeCadena = 0;
	
	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisisca para
	//obtener una lista de claves primarias.
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
	pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
	pipe->agregarParametro(posicion, 3); //Numero de lista a eliminar dentro del archivo.

	//Iniciar comunicacion con la Capa Fisica
	pipe->lanzar();

	//Obtener tamaño de la lista
	pipe->leer(&cantClaves);

	if (cantClaves > 0) {
		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];

		//Leer la lista del archivo.
		pipe->leer(this->getTamanioBloqueLista() - sizeof(unsigned int), bufferClaves);
		
		//Set de claves compuestas
		setClaves = new SetClaves();
		
		//Recorrer todas las claves dentro del buffer.
		for (unsigned char i = 0; i < cantClaves; i++) {
			listaClaves = new ListaClaves();
			//Copia de los valores de las claves en la lista dependiendo de su tipo			
			for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos != this->tipos->end(); ++iterTipos) {

				tipo = (*iterTipos);

				switch(tipo){
					case TipoDatos::TIPO_ENTERO:
					{
						//Copia del valor entero de la clave
						valor = new int;
						tamanio = sizeof(int);
			
						memcpy(valor, bufferClaves, tamanio);
						listaClaves->push_back(new ClaveEntera(*((int*)valor)));
			
						delete (int*)valor;
			
					}break;
					
					case TipoDatos::TIPO_BOOL:
					{
						//Copia del valor booleano de la clave
						valor = new bool;
						tamanio = sizeof(bool);
			
						memcpy(valor, bufferClaves, tamanio);
						listaClaves->push_back(new ClaveBoolean(*((bool*)valor)));
			
						delete (bool*)valor;
						
					}break;
					
					case TipoDatos::TIPO_CHAR:
					{
						//Copia del valor tipo char de la clave.
						valor = new char;
						tamanio = sizeof(char);
			
						memcpy(valor, bufferClaves, tamanio);
						listaClaves->push_back(new ClaveChar(*((char*)valor)));
			
						delete (char*)valor;
					
					}break;
					
					case TipoDatos::TIPO_SHORT:
					{
						//Copia del valor short de la clave
						valor = new short;
						tamanio = sizeof(short);
			
						memcpy(valor, bufferClaves, tamanio);
						listaClaves->push_back(new ClaveShort(*((short*)valor)));
			
						delete (short*)valor;
						
					}break;
					
					case TipoDatos::TIPO_FLOAT:
					{
						//Copia del valor float de la clave
						valor = new float;
						tamanio = sizeof(float);
			
						memcpy(valor, bufferClaves, tamanio);
						listaClaves->push_back(new ClaveReal(*((float*)valor)));
			
						delete (float*)valor;	
						
					}break;
					
					case TipoDatos::TIPO_FECHA:
					{
						//Copia del valor tipo TFECHA de la clave.
						valor = new ClaveFecha::TFECHA;
						tamanio = Tamanios::TAMANIO_FECHA;
			
						memcpy(&(((ClaveFecha::TFECHA*)valor)->anio), bufferClaves, sizeof(unsigned short));
						memcpy(&(((ClaveFecha::TFECHA*)valor)->mes), bufferClaves + sizeof(unsigned short), sizeof(unsigned char));
						memcpy(&(((ClaveFecha::TFECHA*)valor)->dia), bufferClaves + sizeof(unsigned short) + sizeof(unsigned char), sizeof(unsigned char));
						
						listaClaves->push_back(new ClaveFecha(*((ClaveFecha::TFECHA*)valor)));
			
						delete (ClaveFecha::TFECHA*)valor;
						
					}break;
					
					case TipoDatos::TIPO_VARIABLE:
					{
						valor = new string;
					
						memcpy(&sizeCadena, bufferClaves, Tamanios::TAMANIO_LONGITUD_CADENA);
						
						cadena = new char[sizeCadena + 1];
						memcpy(cadena, bufferClaves + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena);
						
						*(cadena + sizeCadena) = 0;			
						valor = cadena;
						
						listaClaves->push_back(new ClaveVariable(*((string*)valor)));
						tamanio = Tamanios::TAMANIO_LONGITUD_CADENA + sizeCadena;
						
						delete (string*)valor;	
						
					}break;
					
				}

				bufferClaves += tamanio;
				setClaves->insert(new ClaveCompuesta(*listaClaves));
				
				if (listaClaves)
					delete listaClaves;
			}
		}
	}
	
	if (bufferClaves)
		delete[] bufferClaves;

	return setClaves;
}

char IndiceCompuestoRomanoManager::escribirListaClaves(unsigned int posicion, SetClaves* setClaves)
{
	char resultado           = 0;    //Resultado de la escritura a disco.
	ListaClaves* listaClaves = NULL; //Lista de claves contenidas en una clave compuesta
	Clave* claveCompuesta    = NULL; //Clave compuesta contenida en cada elmento de set.
	Clave* claveSimple       = NULL; //Cada clave simple cotenida en la lista de claves de la clave compuesta.
	char* bufferClaves       = NULL; //Buffer contenedor de toda la lista de claves en disco.	
	int tipo                 = 0;    //Tipo de cada clave en el buffer.
	unsigned int tamanio     = 0;    //Tamanio de cada clave.
	ComuDatos* pipe          = NULL; //Pipe de comunicaciones entre capas.
	
	//Variable de interpretacion de claves tipo fecha.
	ClaveFecha::TFECHA* fecha = NULL;
	
	//Variables de interpretacion de claves variables.
	unsigned char sizeCadena = 0;
	char* cadena             = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
		pipe->agregarParametro(posicion, 3); //Numero de bloque contenedor de la lista a modificar

		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		ListaTipos::const_iterator itTipos; //Iterador de la lista de tipos.

		//Contruccion del buffer con los datos de longitud variables contenidos 
		//en el set.
		for (SetClaves::iterator itSet = setClaves->begin(); itSet != setClaves->end(); ++itSet) {
			
			//Obtener siguiente ClaveCompuesta del set y su lista de claves simples.
			claveCompuesta = (*itSet);
			listaClaves    = ((ClaveCompuesta*)claveCompuesta)->getListaClaves();
			itTipos        = this->tipos->begin();
			 
			//Iterar la lista de claves simples, preguntando por su tipo y copiando su valor al buffer.
			for (ListaClaves::iterator itLista = listaClaves->begin(); itLista != listaClaves->end(); ++itLista) {
				
				//Obtener siguiente clave simple de la lista de claves de la ClaveCompuesta y su tipo.
				claveSimple = (*itLista);	
				tipo        = (*itTipos);	
						
				switch(tipo){
					case TipoDatos::TIPO_FECHA:
						//Copia del valor fecha de la clave al buffer.
						tamanio = Tamanios::TAMANIO_FECHA;
						fecha = (ClaveFecha::TFECHA*)claveSimple->getValor();
				
						memcpy(bufferClaves, &(fecha->anio), sizeof(unsigned short));
						memcpy(bufferClaves + sizeof(unsigned short), &(fecha->mes), sizeof(unsigned char));
						memcpy(bufferClaves + sizeof(unsigned short) + sizeof(unsigned char), &(fecha->dia), sizeof(unsigned char));
					break;
					
					case TipoDatos::TIPO_VARIABLE:
						//Copia del valor variable de la clave al buffer.
						cadena     = (char*)claveSimple->getValor();
						sizeCadena = claveSimple->getTamanioValor();
						
						memcpy(bufferClaves, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
						memcpy(bufferClaves + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
						
						tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
					break;
						
					default:
						//Copia del valor atomico (int, short, char, etc) de la clave al buffer.
						tamanio = claveSimple->getTamanioValor();						
						memcpy(bufferClaves, claveSimple->getValor(), tamanio);					
					break;
				}				
				
				++itTipos;
				bufferClaves += tamanio;
			}
		}

		//Escritura de la cantidad de claves en la lista
		pipe->escribir((int)setClaves->size());

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

int IndiceCompuestoRomanoManager::escribirListaClaves(SetClaves* setClaves)
{
	unsigned int numBloque   = 0;    //Numero de bloque donde fue almacenada la lista en disco.
	ListaClaves* listaClaves = NULL; //Lista de claves contenidas en una clave compuesta
	Clave* claveCompuesta    = NULL; //Clave compuesta contenida en cada elmento de set.
	Clave* claveSimple       = NULL; //Cada clave simple cotenida en la lista de claves de la clave compuesta.
	char* bufferClaves       = NULL; //Buffer contenedor de toda la lista de claves en disco.	
	int tipo                 = 0;    //Tipo de cada clave en el buffer.
	unsigned int tamanio     = 0;    //Tamanio de cada clave.
	ComuDatos* pipe          = NULL; //Pipe de comunicaciones entre capas.
	
	//Variable de interpretacion de claves tipo fecha.
	ClaveFecha::TFECHA* fecha = NULL;
	
	//Variables de interpretacion de claves variables.
	unsigned char sizeCadena = 0;
	char* cadena             = NULL;

	if (setClaves->size() > 0) {
		//Instancia del pipe
		pipe = this->instanciarPipe();

		//Parametros de inicializacion de la Capa Fisisca para
		//obtener una lista de claves primarias.
		pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion
		pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo
		pipe->agregarParametro(this->getTamanioBloqueLista(), 2); //Tamaño del nodo de la lista en disco
		
		//Iniciar comunicacion con la Capa Fisica
		pipe->lanzar();

		bufferClaves = new char[this->getTamanioBloqueLista() - sizeof(unsigned int)];
		ListaTipos::const_iterator itTipos; //Iterador de la lista de tipos.

		//Contruccion del buffer con los datos de longitud variables contenidos 
		//en el set.
		for (SetClaves::iterator itSet = setClaves->begin(); itSet != setClaves->end(); ++itSet) {
			
			//Obtener siguiente ClaveCompuesta del set y su lista de claves simples.
			claveCompuesta = (*itSet);
			listaClaves    = ((ClaveCompuesta*)claveCompuesta)->getListaClaves();
			itTipos        = this->tipos->begin();
			 
			//Iterar la lista de claves simples, preguntando por su tipo y copiando su valor al buffer.
			for (ListaClaves::iterator itLista = listaClaves->begin(); itLista != listaClaves->end(); ++itLista) {
				
				//Obtener siguiente clave simple de la lista de claves de la ClaveCompuesta y su tipo.
				claveSimple = (*itLista);	
				tipo        = (*itTipos);	
						
				switch(tipo){
					case TipoDatos::TIPO_FECHA:
						//Copia del valor fecha de la clave al buffer.
						tamanio = Tamanios::TAMANIO_FECHA;
						fecha = (ClaveFecha::TFECHA*)claveSimple->getValor();
				
						memcpy(bufferClaves, &(fecha->anio), sizeof(unsigned short));
						memcpy(bufferClaves + sizeof(unsigned short), &(fecha->mes), sizeof(unsigned char));
						memcpy(bufferClaves + sizeof(unsigned short) + sizeof(unsigned char), &(fecha->dia), sizeof(unsigned char));
					break;
					
					case TipoDatos::TIPO_VARIABLE:
						//Copia del valor variable de la clave al buffer.
						cadena     = (char*)claveSimple->getValor();
						sizeCadena = claveSimple->getTamanioValor();
						
						memcpy(bufferClaves, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
						memcpy(bufferClaves + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
						
						tamanio = sizeCadena + Tamanios::TAMANIO_LONGITUD_CADENA;
					break;
						
					default:
						//Copia del valor atomico (int, short, char, etc) de la clave al buffer.
						tamanio = claveSimple->getTamanioValor();						
						memcpy(bufferClaves, claveSimple->getValor(), tamanio);					
					break;
				}				
				
				++itTipos;
				bufferClaves += tamanio;
			}
		}

		//Escritura de la cantidad de claves en la lista
		pipe->escribir((int)setClaves->size());

		//Escritura de la lista al archivo.
		pipe->escribir(bufferClaves, this->getTamanioBloqueLista());

		//Obtencion del numero de bloque donde fue guardada la lista
		//en el archivo.
		pipe->leer(&numBloque);
	}

	if (pipe)
		delete pipe;

	if (bufferClaves)
		delete[] bufferClaves;

	return numBloque;
}

