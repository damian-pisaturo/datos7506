////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoIndice y ArchivoIndiceHash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ArchivoIndice.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Define el comportamiento de las clases de manejo
//			 de archivos de indices en disco).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////

		ArchivoIndice::ArchivoIndice(string nombreArchivo, unsigned short tamBloque) :
			ArchivoBase(nombreArchivo, tamBloque)
		{
			this->archivoEL = new ArchivoELFijo(nombreArchivo + ".nfo");
		}
		
		ArchivoIndice::~ArchivoIndice()
		{ 
			if (this->archivoEL)
				delete this->archivoEL;			
		}
		 
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		short ArchivoIndice::escribirBloque(const void* bloque)
		{		
			bool valor = false;
			ArchivoELFijo* archivoEL = static_cast<ArchivoELFijo*>(this->getArchivoEL());
			
			short bloqueLibre = archivoEL->buscarBloqueLibre();
			
			if (bloqueLibre == ResultadosFisica::BLOQUES_OCUPADOS){
				//Si ningun bloque esta libre, appendea al
				//final del archivo. 
				this->posicionarseFin();
				bloqueLibre = this->posicion();
				
				//Se modifica el atributo booleano de control
				//en el archivo de espacio libre.
				archivoEL->agregarRegistro(&valor);

			}else{
				this->posicionarse(bloqueLibre);
				archivoEL->modificarRegistro(&valor, bloqueLibre);
			}
			
			//Escritura del bloque a disco.
			this->escribir(bloque);
			
			return bloqueLibre;
		}

		char ArchivoIndice::escribirBloque(const void* bloque, unsigned short numBloque)
		{			
			char resultado = ResultadosFisica::OK;
			bool valor = false;
			ArchivoELFijo* archivoEL = static_cast<ArchivoELFijo*>(this->getArchivoEL());
			
			resultado = this->posicionarse(numBloque);
			
			if (resultado == ResultadosFisica::OK){
				
				resultado = this->escribir(bloque);
			
				//Se modifica la entrada correspondiente al bloque 
				//modificado en el archivo de control de espacio libre.
				archivoEL->modificarRegistro(&valor, numBloque);
			}
			
			return resultado;
		}
		
		char ArchivoIndice::eliminarBloque(unsigned short numBloque)
		{
			bool valor = true;
			ArchivoEL* archivoEL = this->getArchivoEL();
			
			return archivoEL->modificarRegistro(&valor, numBloque);			
		}
		
		char ArchivoIndice::leerBloque(void* bloque, unsigned short numBloque)
		{	
			char resultado = this->posicionarse(numBloque);
			
			if (resultado == ResultadosFisica::OK)
				resultado = this->leer(bloque);
			
			return resultado;			
		}	 

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoLista
//			(Permite el manejo de archivos contenedores de listas de 
//			claves primarias utilizadas por los indices secundarios).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoLista::ArchivoLista(string nombreArchivo, unsigned short tamBloqueLista):
			ArchivoIndice(nombreArchivo, tamBloqueLista)
		{ }
		
		ArchivoLista::~ArchivoLista()
		{ }
		
	///////////////////////////////////////////////////////////////////////
 	//	Metodos publicos
 	///////////////////////////////////////////////////////////////////////
		
		short ArchivoLista::escribirLista(const void* lista)
		{	
			return this->escribirBloque(lista);
		}				
	
		char ArchivoLista::escribirLista(const void* lista, unsigned short numBloque)
		{
			return this->escribirBloque(lista, numBloque);
		}
				
		char ArchivoLista::eliminarLista(unsigned short numBloque)
		{
			return this->eliminarBloque(numBloque);
		}
				
		char ArchivoLista::leerLista(void* lista, unsigned short numBloque)
		{
			return this->leerBloque(lista, numBloque);
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoTablaHash
//			(Permite el manejo de archivos contenedores de los datos
//			de la tabla de dispersion utilizada en los indices de Hash).
///////////////////////////////////////////////////////////////////////////
		
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoTablaHash::ArchivoTablaHash(string nombreArchivo):
			ArchivoBase(nombreArchivo, sizeof(unsigned int))
		{
			//Escribe una cantidad de elementos inicial igual
			//a cero, si la tabla se crea por primera vez, y un
			//elemento inicial.
			if (this->size() == Tamanios::TAMANIO_IDENTIFICADOR){
				unsigned int valorNulo = 0;
				this->escribir(&valorNulo);
				this->posicionarseFin();
				this->escribir(&valorNulo);				
			}
		}
		ArchivoTablaHash::~ArchivoTablaHash() { }
		 
	///////////////////////////////////////////////////////////////////////
 	//	Metodos publicos
 	///////////////////////////////////////////////////////////////////////
		 unsigned int ArchivoTablaHash::obtenerCantElementos()
		 {
			 unsigned int cantElementos = 0;
			 
			 //Posicionarse al comienzo del archivo
			 this->posicionarse(0);
			
			 //Obtener el primer dato entero de la tabla
			 this->leer(&cantElementos);
			
			 return cantElementos; 
		 }
		 
		 unsigned int* ArchivoTablaHash::obtenerTabla(unsigned int cantElementos)
		 {
			 unsigned int* tabla = NULL;			 
			 
			 if (cantElementos > 0){
			 	tabla = new unsigned int[cantElementos + 1];
			 
				 for (unsigned int i = 0; i < cantElementos; i++){
					 this->posicionarse(i + 1);
					 this->leer(tabla + i);
				 }
			 }
			 
			 return tabla;
		 }
		 
		 char ArchivoTablaHash::escribirCantElementos(unsigned int cantElem)
		 {	
			 char resultado = this->posicionarse(0);
			 
			 //Posicionarse al comienzo del archivo.
			 if (resultado == ResultadosFisica::OK)			 
				 //Obtener el primer atributo entero (cantidad de elementos)
				 resultado = this->escribir(&cantElem);	
			 
			 return resultado;
		 }
		 
		 char ArchivoTablaHash::escribirTabla(unsigned int* tabla)
		 {
			 char resultado = ResultadosFisica::OK;
			 unsigned int cantElem = this->obtenerCantElementos();
			 			 
			 //Posicionarse en el segundo atributo entero
			 //(primer elemento de la tabla)
			resultado = this->posicionarse(1);
			
			 //Recorrer todos los elementos de tabla y
			 //almacenarlos a disco.
			 for (unsigned int i = 0; ( (i < cantElem) && (resultado == ResultadosFisica::OK) ); i++)
				 resultado = this->escribir(tabla + i);

			 return resultado;			 
		 } 
		
///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoIndiceHash
//			(Permite el manejo de archivos de indices que emplean 
//			funciones de dispersion extensible).
///////////////////////////////////////////////////////////////////////////
		 
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndiceHash::ArchivoIndiceHash(string nombreArchivo, unsigned short tamBucket):
			ArchivoIndice(nombreArchivo, tamBucket)
		{
			this->archivoTabla = new ArchivoTablaHash(nombreArchivo + ".tlb");
		}
		
		 ArchivoIndiceHash::~ArchivoIndiceHash() 
		 { 
			 if (this->archivoTabla)
				 delete this->archivoTabla; 
		 }
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
				
		void ArchivoIndiceHash::leerTabla(unsigned int* cantElem, unsigned int* &elementos)
		{			
			*cantElem = this->archivoTabla->obtenerCantElementos();
			elementos = this->archivoTabla->obtenerTabla(*cantElem);
		}
		
		void ArchivoIndiceHash::escribirTabla(unsigned int cantElem, unsigned int* elementos)
		{
			this->archivoTabla->escribirCantElementos(cantElem);
			this->archivoTabla->escribirTabla(elementos);			
		}
		

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoIndiceArbol
//			(Permite el manejo de archivos empleados en la persistencia de
//			datos a traves de indices con estructura de arbol B+ o B*).
///////////////////////////////////////////////////////////////////////////
 
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndiceArbol::ArchivoIndiceArbol(string nombreArchivo, unsigned short tamBucket):
			ArchivoIndice(nombreArchivo, tamBucket)
			{ }
		
		ArchivoIndiceArbol::~ArchivoIndiceArbol() { }
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////

		short ArchivoIndiceArbol::escribirBloqueDoble(const void* nodo)
		{
			bool valor = false;
			ArchivoELFijo* archivoEL = static_cast<ArchivoELFijo*>(this->getArchivoEL());
			
			short bloqueLibre = archivoEL->buscarBloqueDobleLibre();
			
			if (bloqueLibre == ResultadosFisica::BLOQUES_OCUPADOS){
				//Si ningun par de bloques consecutivos esta libre, appendea al
				//final del archivo. 
				this->posicionarseFin();
				bloqueLibre = this->posicion();
				
				//Se modifica el atributo booleano de control
				//en el archivo de espacio libre.
				archivoEL->agregarRegistro(&valor);

			}else{ 
				this->posicionarse(bloqueLibre);
				archivoEL->modificarRegistro(&valor, bloqueLibre);
				archivoEL->modificarRegistro(&valor, bloqueLibre + 1);
			}
			
			//Escritura del nodo doble a disco.
			this->escribir(nodo);
			this->escribir((char*)nodo + this->getTamanioBloque());
			
			return bloqueLibre;
		}
		
		
		char ArchivoIndiceArbol::escribirBloqueDoble(const void* nodo, unsigned short numBloque)
		{
			char resultado = ResultadosFisica::OK;
			bool valor     = false;
			ArchivoELFijo* archivoEL = NULL;
			
			resultado = this->posicionarse(numBloque);
			
			if (resultado == ResultadosFisica::OK){
				resultado = this->escribir(nodo);
				
				if (resultado == ResultadosFisica::OK){
					resultado = this->escribir((char*)nodo + this->getTamanioBloque());
					
					//Se modifica la entrada correspondiente al bloque 
					//modificado en el archivo de control de espacio libre.
					archivoEL = static_cast<ArchivoELFijo*>(this->getArchivoEL());
					archivoEL->modificarRegistro(&valor, numBloque);
					archivoEL->modificarRegistro(&valor, numBloque + 1);
					
				}
			}
			
			return resultado;
		}
		

		char ArchivoIndiceArbol::eliminarBloqueDoble(unsigned short numNodo)
		{
			char resultado = this->eliminarBloque(numNodo);
			
			if (resultado == ResultadosFisica::OK)
				resultado = this->eliminarBloque(numNodo + 1);
			
			return resultado;
		}
		
		char ArchivoIndiceArbol::leerBloqueDoble(void* nodo, unsigned short numNodo)
		{
			char resultado = this->leerBloque(nodo, numNodo);
			
			if (resultado == ResultadosFisica::OK)
				resultado = this->leerBloque((char*)nodo + this->getTamanioBloque(), numNodo + 1);
			
			return resultado;
		}
