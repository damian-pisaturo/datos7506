///////////////////////////////////////////////////////////////////////////
//	Archivo   : IndiceManager.cpp
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases IndiceManager correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string).
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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
			
			//Se obtiene el resultado de la operacion de lectura.
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
		}
	}

	if (pipe)
		delete pipe;
	
	if (punteroAux)
		delete[] punteroAux;

	return resultado;
}

int IndiceArbolManager::escribirBloque(BloqueIndice* bloqueNuevo) 
{
	char resultado  = 0;
	int numBloque   = 0;
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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
	
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
			pipe->leer(&numBloque);
		
			//Setear en el nodo la posicion donde se grabo el nodo.
			nodoNuevo->setPosicionEnArchivo(numBloque);
		}else
			numBloque = resultado;
	}else
		numBloque = resultado;

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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
	
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
		}
	}

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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
	
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK)	
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
	
		if (pipe)
			delete pipe;
	}

	return resultado;
}

int IndiceArbolManager::escribirBloqueDoble(BloqueIndice* bloqueNuevo)
{
	char resultado  = 0;
	int numBloque   = 0;
	Nodo* nodoNuevo = static_cast<Nodo*> (bloqueNuevo);

	//Variables de escritura del buffer
	char* data = new char[2*this->getTamanioBloque()];
	char* punteroAux = data;
	memset(data, 0, 2*this->getTamanioBloque());

	//Variables de interpretacion del nodo
	HeaderNodo headerNodo;
	SetClaves* set = NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica.
	pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_NODO_DOBLE, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.

	//Se lanza el proceso de la capa fisica. 
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
		
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
		}else
			numBloque = resultado;
	}else numBloque = resultado;

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
	SetClaves* set = NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica.
	pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_NODO_DOBLE, 0); //Codigo de operacion.
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
	pipe->agregarParametro(numBloque, 3); // Numero del primer nodo a escribir a disco.

	//Se lanza el proceso de la capa fisica. 
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){	
		
			//Copiar el header al buffer
			headerNodo.nivel = nodoNuevo->getNivel();
			headerNodo.refNodo = nodoNuevo->getRefNodo();
			headerNodo.espacioLibre = nodoNuevo->getEspacioLibre();
		
			memcpy(data, &headerNodo.nivel, sizeof(char));
			data+=sizeof(char);
			
			//El hijo izq se guarda siempre y cuando no se trate de un nodo hoja de un árbol B*
			if (!((this->getTipoIndice() == TipoIndices::ARBOL_BS) && (headerNodo.nivel == 0))) {
				memcpy(data, &headerNodo.refNodo, sizeof(unsigned int));
				data += sizeof(unsigned int);
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
		}
	}

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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
		
		
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
				
				//Recorrer el buffer desde donde quedo hasta que supere
				//el espacio libre, interpretando clave por clave.
				const char* punteroFinal = punteroAux + (2*this->getTamanioBloque() - headerNodo.espacioLibre);
		
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
		}
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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK)
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
	}

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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
		
			//Se obtiene el resultado de la operacion de lectura
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
		}
	}

	if (pipe)
		delete pipe;

	return resultado;
}

int IndiceHashManager::escribirBloque(BloqueIndice* nuevoBloque)
{
	char resultado      = 0;
	int numBucket       = 0;
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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
	
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
	
			data = bucketLeido->getDatos();
		
			//Grabar el buffer en el archivo
			pipe->escribir(data, this->getTamanioBloque());
		
			//Obtener nueva posicion del bucket en el archivo. 
			pipe->leer(&numBucket);
		
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
		}else 
			numBucket = resultado;
	}else numBucket = resultado;

	if (pipe)
		delete pipe;

	return numBucket;
}

int IndiceHashManager::escribirBloque(unsigned short numBucket, BloqueIndice* bloqueModif) 
{
	char resultado      = ResultadosFisica::OK;
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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){	
			buffer = bucketLeido->getDatos();
		
			//Grabar el buffer en el archivo.
			pipe->escribir(buffer, this->getTamanioBloque());
		
			//Setear en el bucket la posicion donde se grabo.
			bucketLeido->setNroBloque(numBucket);
		
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
		}
	}

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
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK)	
			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);
	}

	if (pipe)
		delete pipe;

	return resultado;
}

char IndiceHashManager::leerTabla(unsigned int* tamanio, unsigned int* &buckets) 
{
	char resultado     = 0;
	char* bucketsTabla = NULL;

	//Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	//Parametros de inicializacion de la Capa Fisica para
	//leer la tabla de dispersion 
	pipe->agregarParametro(OperacionesCapas::FISICA_LEER_TABLA_HASH, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.

	//Se lanza el proceso de la capa fisica. 
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
			//Obtener tamaño de la tabla
			pipe->leer(tamanio);
		
			if (tamanio > 0) {
				unsigned int sizeTabla = (sizeof(unsigned int))*(*tamanio);
		
				bucketsTabla = new char[sizeTabla];
		
				//Obtener datos de la tabla				
				pipe->leer(sizeTabla, bucketsTabla);
			}
		
			buckets = (unsigned int*) bucketsTabla;
		}
	}

	if (pipe)
		delete pipe;
	
	return resultado;
}

char IndiceHashManager::escribirTabla(unsigned int tamanio, unsigned int* buckets) 
{
	char resultado     = ResultadosFisica::OK;
	char* bucketsTabla = NULL;

	// Instancia del pipe
	ComuDatos* pipe = this->instanciarPipe();

	// Parametros de inicializacion de la Capa Fisica para
	// actualizar la tabla de dispersion 
	pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_TABLA_HASH, 0); //Codigo de operacion
	pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
	pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño de un bucket en disco.

	// Se lanza el proceso de la capa fisica. 
	resultado = pipe->lanzar();
	
	if (resultado == ComuDatos::SIN_ERROR){
		
		//Se chequea la validez del archivo
		pipe->leer(&resultado);
		
		if (resultado == ResultadosFisica::OK){
	
		
			// Enviar el tamaño de la tabla por el pipe.
			pipe->escribir(tamanio);
		
			bucketsTabla = (char*) buckets;
		
			// Enviar el contenido de la tabla por el pipe.
			pipe->escribir(bucketsTabla, tamanio*sizeof(unsigned int));
			
			// Obtener el resultado de la operacion.
			pipe->leer(&resultado);
		}
	}
		
	if (pipe)
		delete pipe;
	
	return resultado;
}


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceEnteroManager 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceEnteroManager::IndiceEnteroManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceEnteroManager::leerClaveHoja(char* &buffer)
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

Clave* IndiceEnteroManager::leerClaveNoHoja(char* &buffer) 
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
// Nombre: IndiceBooleanManager
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceBooleanManager::IndiceBooleanManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////			
Clave* IndiceBooleanManager::leerClaveHoja(char* &buffer) 
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

Clave* IndiceBooleanManager::leerClaveNoHoja(char* &buffer)
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
// Nombre: IndiceCharManager
//		   (Implementa archivo de indices de clave tipo char).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceCharManager::IndiceCharManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////		
Clave* IndiceCharManager::leerClaveHoja(char* &buffer)
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

Clave* IndiceCharManager::leerClaveNoHoja(char* &buffer) 
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

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nombre: IndiceShortManager
//		   (Implementa archivo de indices de clave de tipo short).
//////////////////////////////////////////////////////////////////////////////	

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceShortManager::IndiceShortManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceShortManager::leerClaveHoja(char* &buffer) 
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

Clave* IndiceShortManager::leerClaveNoHoja(char* &buffer) 
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


//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceRealManager
//		   (Implementa archivo de indices de clave de tipo float).
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceRealManager::IndiceRealManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceRealManager::leerClaveHoja(char* &buffer) 
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

Clave* IndiceRealManager::leerClaveNoHoja(char* &buffer) 
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

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaManager
//		   (Implementa archivo de indices de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceFechaManager::IndiceFechaManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	
Clave* IndiceFechaManager::leerClaveHoja(char* &buffer) 
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

Clave* IndiceFechaManager::leerClaveNoHoja(char* &buffer) 
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
// Nombre: IndiceVariableManager 
//		   (Implementa archivo de indices de clave de longitud variable).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceVariableManager::IndiceVariableManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
	{ }

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////	

void IndiceVariableManager::copiarClaveHoja(Clave* clave, char* &buffer) 
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

void IndiceVariableManager::copiarClaveNoHoja(Clave* clave, char* &buffer) 
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

Clave* IndiceVariableManager::leerClaveHoja(char* &buffer) 
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

Clave* IndiceVariableManager::leerClaveNoHoja(char* &buffer) 
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
// Nombre: IndiceCompuestoManager 
//		   (Implementa archivo de indices de clave compuesta).
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
IndiceCompuestoManager::IndiceCompuestoManager(unsigned int tamNodo, string nombreArchivo, 
		unsigned char tipoIndice, ListaTipos* listaTipos) :
	IndiceArbolManager(tamNodo, nombreArchivo, tipoIndice) 
{ 
	this->tipos = listaTipos;
}

//////////////////////////////////////////////////////////////////////
// Metodos privados
//////////////////////////////////////////////////////////////////////

void IndiceCompuestoManager::copiarClaveHoja(Clave* clave, char* &buffer) 
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
		
		if (*iterTipos == TipoDatos::TIPO_STRING){
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

void IndiceCompuestoManager::copiarClaveNoHoja(Clave* clave, char* &buffer) 
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
		
		if (*iterTipos == TipoDatos::TIPO_STRING){
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

Clave* IndiceCompuestoManager::leerClaveHoja(char* &buffer) 
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

		switch(tipo){
	
			case TipoDatos::TIPO_ENTERO:
			{
				//Copia del valor de la clave
				valor = new int;
				tamanio = sizeof(int);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveEntera(*((int*)valor)));
				
			}break;
			
			case TipoDatos::TIPO_BOOL:
			{
				valor = new bool;
				tamanio = sizeof(bool);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
				
			}break;
			
			case TipoDatos::TIPO_CHAR:
			{
				valor = new char;
				tamanio = sizeof(char);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveChar(*((char*)valor)));
			
			}break;
			
			case TipoDatos::TIPO_SHORT:
			{
				valor = new short;
				tamanio = sizeof(short);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveShort(*((short*)valor)));
			}break;
			
			case TipoDatos::TIPO_FLOAT:
			{
				valor = new float;
				tamanio = sizeof(float);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveReal(*((float*)valor)));		
			}break;
			
			case TipoDatos::TIPO_FECHA:
			{
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
			}break;
			
			case TipoDatos::TIPO_STRING:
			{
				valor = new string;
							
				memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
				
				cadena = new char[sizeCadena + 1];
				memcpy(cadena, buffer + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena);
				
				*(cadena + sizeCadena) = 0;			
				valor = cadena;
				
				listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				
				if (valor)
					delete (string*)valor;
				
				if (cadena)
					delete[] cadena;
										
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

Clave* IndiceCompuestoManager::leerClaveNoHoja(char* &buffer)
{
	ListaClaves listaClaves;
	int tipo    = 0;
	unsigned int refRegistro = 0, hijoDer = 0, tamanio = 0;
	void* valor = NULL;
	
	//Variables de interpretacion de claves de longitud
	//variables
	char* cadena             = NULL;
	unsigned char sizeCadena = 0;

	//Copia de los valores de las claves en el nodo				
	for (ListaTipos::const_iterator iterTipos = this->tipos->begin(); iterTipos
			!= this->tipos->end(); ++iterTipos) {

		tipo = (*iterTipos);

		switch(tipo){
	
			case TipoDatos::TIPO_ENTERO:
			{
				//Copia del valor de la clave
				valor = new int;
				tamanio = sizeof(int);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveEntera(*((int*)valor)));
				
			}break;
			
			case TipoDatos::TIPO_BOOL:
			{
				valor = new bool;
				tamanio = sizeof(bool);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
				
			}break;
			
			case TipoDatos::TIPO_CHAR:
			{
				valor = new char;
				tamanio = sizeof(char);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveChar(*((char*)valor)));
			
			}break;
			
			case TipoDatos::TIPO_SHORT:
			{
				valor = new short;
				tamanio = sizeof(short);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveShort(*((short*)valor)));
			}break;
			
			case TipoDatos::TIPO_FLOAT:
			{
				valor = new float;
				tamanio = sizeof(float);
		
				memcpy(valor, buffer, tamanio);
				listaClaves.push_back(new ClaveReal(*((float*)valor)));		
			}break;
			
			case TipoDatos::TIPO_FECHA:
			{
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
			}break;
			
			case TipoDatos::TIPO_STRING:
			{
				valor = new string;
							
				memcpy(&sizeCadena, buffer, Tamanios::TAMANIO_LONGITUD_CADENA);
				
				cadena = new char[sizeCadena + 1];
				memcpy(cadena, buffer + Tamanios::TAMANIO_LONGITUD_CADENA, sizeCadena);
				
				*(cadena + sizeCadena) = 0;			
				valor = cadena;
				
				listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				
				if (valor)
					delete (string*)valor;	
				
				if (cadena)
					delete[] cadena;
										
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
