////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoDatos,
//		ArchivoDatosBloques, ArchivosDatosRegistros.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "ArchivoDatos.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoDatos
//			(Implementa primitivas para el manejo de archivos
//           de datos en disco).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoDatos::ArchivoDatos(string nombreArchivo, unsigned short tamanio, unsigned char tipoOrg):
			ArchivoBase(nombreArchivo, tamanio)
		{ 
			//Creacion e inicializacion del archivo de control de 
			//espacio libre.
			bool valorBool;
			unsigned short valorShort;
			
			switch(tipoOrg){
			case (TipoOrganizacion::REG_FIJOS):
				valorBool = true;
				this->archivoEL = new ArchivoELFijo(nombreArchivo + ".nfo");
				
				//Escribir primer valor booleano en el archivo de control.
				this->archivoEL->escribir(&valorBool);
				
				break;
			case (TipoOrganizacion::REG_VARIABLES):
				valorShort = tamanio;
				this->archivoEL = new ArchivoELVariable(nombreArchivo + ".nfo");
				
				//Escribir primer valor de espacio libre en el archivo de control.
				this->archivoEL->escribir(&valorShort);
				
				break;				
			default:
				this->archivoEL = NULL;	
				
				break;
			}			
		}
		
		ArchivoDatos::~ArchivoDatos()
		{
			if (this->archivoEL)
				delete archivoEL;
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoDatosBloques
//			(Implementa primitivas para el manejo de archivos
//           de datos con organizacion de registros variables en disco).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoDatosBloques::ArchivoDatosBloques(string nombreArchivo, unsigned short tamBloque):
			ArchivoDatos(nombreArchivo, tamBloque, TipoOrganizacion::REG_VARIABLES)
		{ }
		
		ArchivoDatosBloques::~ArchivoDatosBloques(){ }
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		char ArchivoDatosBloques::leerBloque(void* bloque, unsigned short numBloque)
		{
			char resultado = this->posicionarse(numBloque);
			
			if (resultado == ResFisica::OK)
				resultado = this->leer(bloque);
			
			return resultado;
		}
		
		short ArchivoDatosBloques::leerBloque(void* bloque, unsigned int espacioLibre)
		{
			short resultado = ResFisica::OK;
			unsigned short posicion = 0;
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			
			posicion = archivoEL->buscarEspacioLibre(espacioLibre);
			
			if (posicion != ResFisica::BLOQUES_OCUPADOS){
				resultado = this->posicionarse(posicion);
				if (resultado == ResFisica::OK)
					resultado = this->leer(bloque);
			}
			
			if (resultado != ResFisica::OK)
				posicion = resultado;
			
			return posicion;				
		}

		char ArchivoDatosBloques::escribirBloque(void* bloque, unsigned short numBloque)
		{
			char resultado = this->posicionarse(numBloque);
			
			if (resultado == ResFisica::OK)
				resultado = this->escribir(bloque);
			
			return resultado;			
		}
		
		short ArchivoDatosBloques::escribirBloque(void* bloque)
		{
			//TODO buscarBloqueLibre() tiene que buscar
			//si existe alguna entrada cuyo espacio libre
			//coincida con el tamanio del bloque.
			
			//ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			short bloqueLibre;// = archivoEL->buscarBloqueLibre();
			
			if (bloqueLibre == ResFisica::BLOQUES_OCUPADOS){
				this->posicionarseFin();
				bloqueLibre = this->posicion() + 1;
			}else
				this->posicionarse(bloqueLibre);
			
			this->escribir(bloque);
			
			return bloqueLibre;
		}
		
		char ArchivoDatosBloques::eliminarBloque(unsigned short numBloque)
		{
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			unsigned short tamBloque = this->getTamanioBloque();
			
			return archivoEL->modificarRegistro(&tamBloque, numBloque);			
		}
