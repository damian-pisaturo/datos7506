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
#include "ArchivoEL.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Abstracta. Define el comportamiento de las clases de manejo
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
		
		ArchivoIndice::~ArchivoIndice() { }
		 
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		unsigned short ArchivoIndice::insertarBloque(void* bloque)
		{			
			ArchivoELFijo* archivoEL = this->getArchivoEL();
			
			unsigned short bloqueLibre = archivoEL->buscarBloqueLibre();
			
			if (bloqueLibre == CodArchivo::BLOQUES_OCUPADOS){
				this->posicionarseFin();
				bloqueLibre = this->posicion() + 1;
			}else
				this->posicionarse(bloqueLibre);
			
			this->escribir(bloque);			
			
			return bloqueLibre;
		}

		char ArchivoIndice::modificarBloque(void* bloque, unsigned short numBloque)
		{
			char resultado = CodArchivo::OK;
			
			resultado = this->posicionarse(numBloque);
			if (resultado == CodArchivo::OK)
				resultado = this->escribir(bloque);
			
			return resultado;
		}
		
		char ArchivoIndice::eliminarBloque(unsigned short numBloque)
		{
			bool valor = false;
			ArchivoEL* archivoEL = this->getArchivoEL();
			
			return archivoEL->modificarRegistro(&valor, numBloque);			
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
		virtual ~ArchivoIndiceHash::~ArchivoIndiceHash();
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
				
		void ArchivoIndiceHash::leerTabla(unsigned short* cantElem, unsigned int* elementos)
		{
			
			
		}
		
		void ArchivoIndiceHash::escribirTabla(unsigned short cantElem, unsigned int* elementos)
		{
			
		}
				

		
		
				

		
