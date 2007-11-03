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
			//Creacion e inicializacion del archivo de control
			//de espacio libre.
			bool valor = true;
			this->archivoEL = new ArchivoELFijo(nombreArchivo + ".nfo");
			
			//Escribir primer valor booleano en el archivo de control.
			this->archivoEL->escribir(&valor);
		}
		
		ArchivoIndice::~ArchivoIndice()
		{ 
			if (this->archivoEL)
				delete archivoEL;			
		}
		 
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		short ArchivoIndice::escribirBloque(const void* bloque)
		{			
			ArchivoELFijo* archivoEL = static_cast<ArchivoELFijo*>(this->getArchivoEL());
			
			short bloqueLibre = archivoEL->buscarBloqueLibre();
			
			if (bloqueLibre == ResFisica::BLOQUES_OCUPADOS){
				this->posicionarseFin();
				bloqueLibre = this->posicion() + 1;
			}else
				this->posicionarse(bloqueLibre);
			
			this->escribir(bloque);			
			
			return bloqueLibre;
		}

		char ArchivoIndice::escribirBloque(const void* bloque, unsigned short numBloque)
		{
			char resultado = ResFisica::OK;
			
			resultado = this->posicionarse(numBloque);
			if (resultado == ResFisica::OK)
				resultado = this->escribir(bloque);
			
			return resultado;
		}
		
		char ArchivoIndice::eliminarBloque(unsigned short numBloque)
		{
			bool valor = false;
			ArchivoEL* archivoEL = this->getArchivoEL();
			
			return archivoEL->modificarRegistro(&valor, numBloque);			
		}
		
		char ArchivoIndice::leerBloque(void* bloque, unsigned short numBloque)
		{	
			char resultado = this->posicionarse(numBloque);
			
			if (resultado == ResFisica::OK)
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
			ArchivoIndice(nombreArchivo + ".list", tamBloqueLista)
		{ }
		
		ArchivoLista::~ArchivoLista()
		{ }
		
	///////////////////////////////////////////////////////////////////////
 	//	Metodos publicos
 	///////////////////////////////////////////////////////////////////////
		
		short ArchivoLista::escribirLista(const unsigned int cantClaves, const void* lista)
		{
			short posicion = 0;
			
			char* datos = new char[this->getTamanioBloque()];
			
			*((unsigned int*)datos) = cantClaves;
			memcpy(datos, lista, this->getTamanioBloque() - sizeof(unsigned int));
		
			posicion = this->escribirBloque(datos);
			
			delete[] datos;
			
			return posicion;
		}				
	
		char ArchivoLista::escribirLista(const unsigned int cantClaves, const void* lista, unsigned short numBloque)
		{
			char resultado = ResFisica::OK;
			
			char* datos = new char[this->getTamanioBloque()];
			
			*((unsigned int*)datos) = cantClaves;
			memcpy(datos, lista, this->getTamanioBloque() - sizeof(unsigned int));
			
			resultado = this->escribirBloque(datos, numBloque);
			
			delete[] datos;
			
			return resultado;
		}
				
		char ArchivoLista::eliminarLista(unsigned short numBloque)
		{
			return this->eliminarBloque(numBloque);
		}
				
		char ArchivoLista::leerLista(unsigned int* cantClaves, void* lista, unsigned short numBloque)
		{
			char resultado = ResFisica::OK;
			char* datos = new char[this->getTamanioBloque() + 1];
			 
			resultado = this->leerBloque(datos, numBloque);
			
			if (resultado = ResFisica::OK){
				cantClaves = ((unsigned int*)datos);
				memcpy(lista, datos + sizeof(unsigned int), this->getTamanioBloque() - sizeof(unsigned int));			
			}
			
			delete[] datos;
			
			return resultado;
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
				//a cero.
				unsigned int cantElementos = 0;
				this->escribir(&cantElementos);
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
		 
		 unsigned int* ArchivoTablaHash::obtenerTabla()
		 {
			 unsigned int* tabla = NULL;
			 unsigned int cantElementos = obtenerCantElementos();			 
			 
			 if (cantElementos > 0){
			 	tabla = new unsigned int[cantElementos + 1];
			 
				 for (unsigned int i = 0; i < cantElementos; i++){
					 this->posicionarse(i + 1);
					 this->leer(tabla + i);
				 }
			 }
			 
			 return tabla;
		 }
		 
		 char ArchivoTablaHash::escribirCantElementos(unsigned short cantElem)
		 {	
			 char resultado = this->posicionarse(0);
			 
			 //Posicionarse al comienzo del archivo.
			 if (resultado == ResFisica::OK)			 
				 //Obtener el primer atributo entero (cantidad de elementos)
				 resultado = this->escribir(&cantElem);	
			 
			 return resultado;
		 }
		 
		 char ArchivoTablaHash::escribirTabla(unsigned int* tabla)
		 {
			 char resultado = ResFisica::OK;
			 unsigned int cantElem = this->obtenerCantElementos();
			 
			 //Posicionarse en el segundo atributo entero
			 //(primer elemento de la tabla)
			 resultado = this->posicionarse(1);
			 
			 //Recorrer todos los elementos de tabla y
			 //almacenarlos a disco.
			 if (resultado == ResFisica::OK)
				 for (unsigned int i = 0; ( (i < cantElem) && (resultado == ResFisica::OK) ); i++){
					 resultado = this->escribir(tabla + i);
					 if (resultado == ResFisica::OK)
						 resultado = this->posicionarse(i + 2);
				 }
			 
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
				archivoTabla = new ArchivoTablaHash(nombreArchivo + ".nfo");
			}
		
		 ArchivoIndiceHash::~ArchivoIndiceHash() { }
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
				
		void ArchivoIndiceHash::leerTabla(unsigned short* cantElem, unsigned int* elementos)
		{			
			*cantElem = this->archivoTabla->obtenerCantElementos();
			elementos = this->archivoTabla->obtenerTabla();
		}
		
		void ArchivoIndiceHash::escribirTabla(unsigned short cantElem, unsigned int* elementos)
		{
			this->archivoTabla->escribirCantElementos(cantElem);
			this->archivoTabla->escribirTabla(elementos);			
		}
				

		
		
				

		
