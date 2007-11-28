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
			
			if (resultado == ResultadosFisica::OK)
				resultado = this->leer(bloque);
			
			return resultado;
		}
		
		short ArchivoDatosBloques::buscarBloque(void* bloque, unsigned short espacioLibre, unsigned short numBloque)
		{
			short resultado = ResultadosFisica::OK;
			short posicion  = 0;
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			
			posicion = archivoEL->buscarEspacioLibre(espacioLibre, numBloque);
			
			if (posicion != ResultadosFisica::BLOQUES_OCUPADOS){
				resultado = this->posicionarse(posicion);
				
				if ( (resultado == ResultadosFisica::OK) && 
						(this->size() > Tamanios::TAMANIO_IDENTIFICADOR) ){
					this->posicionarse(posicion);
					resultado = this->leer(bloque);
				}else resultado = ResultadosFisica::ARCHIVO_VACIO;
			}
			
			if (resultado != ResultadosFisica::OK)
				posicion = resultado;
			
			return posicion;				
		}

		char ArchivoDatosBloques::escribirBloque(void* bloque, unsigned short numBloque, unsigned short espLibre)
		{
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			char resultado = this->posicionarse(numBloque);
			
			if (resultado == ResultadosFisica::OK){
				resultado = this->escribir(bloque);
				
				//Se modifica la entrada correspondiente al bloque 
				//modificado en el archivo de control de espacio libre.
				archivoEL->modificarRegistro(&espLibre, numBloque);				
			}
			
			return resultado;			
		}
		
		short ArchivoDatosBloques::escribirBloque(void* bloque, unsigned short espLibre)
		{			
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			short bloqueLibre = archivoEL->buscarEspacioLibre(this->getTamanioBloque());
			
			if (bloqueLibre == ResultadosFisica::BLOQUES_OCUPADOS){
				//Si ningun bloque esta libre, appendea al
				//final del archivo.
				this->posicionarseFin();
				bloqueLibre = this->posicion();
				
				//Se modifica el atributo booleano de control
				//en el archivo de espacio libre.
				archivoEL->agregarRegistro(&espLibre);
				
			}else{				
				archivoEL->modificarRegistro(&espLibre, bloqueLibre);
				this->posicionarse(bloqueLibre);
			}			
			
			this->escribir(bloque);
			
			return bloqueLibre;
		}
		
		char ArchivoDatosBloques::eliminarBloque(unsigned short numBloque)
		{
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			unsigned short tamBloque = this->getTamanioBloque();
			
			return archivoEL->modificarRegistro(&tamBloque, numBloque);			
		}
		
		char ArchivoDatosBloques::siguienteBloque(void* bloque, int& numBloque, unsigned short espLibre)
		{			
			char resultado	= ResultadosFisica::OK;
			int posicion  = 0;
			
			ArchivoELVariable* archivoEL = static_cast<ArchivoELVariable*>(this->getArchivoEL());
			
			posicion = archivoEL->buscarBloqueOcupado(espLibre, numBloque);
			
			if (posicion != ResultadosFisica::BLOQUES_VACIOS) {
				
				resultado = this->posicionarse(posicion);
				
				if ( (resultado == ResultadosFisica::OK) && 
					 (this->size() > Tamanios::TAMANIO_IDENTIFICADOR) ) {
					this->posicionarse(posicion);
					resultado = this->leer(bloque);
					numBloque = posicion;
				} else resultado = ResultadosFisica::FIN_BLOQUES;
				
			} else resultado = ResultadosFisica::FIN_BLOQUES;
			
			return resultado;
		}
		

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoDatosRegistros
//			(Implementa primitivas para el manejo de archivos
//           de datos con organizacion de registros de longitud fija 
//			 en disco).
///////////////////////////////////////////////////////////////////////////
		ArchivoDatosRegistros::ArchivoDatosRegistros(string nombreArchivo, unsigned short tamReg):
			ArchivoDatos(nombreArchivo, tamReg, TipoOrganizacion::REG_FIJOS)
		{ }
		
		ArchivoDatosRegistros::~ArchivoDatosRegistros()
		{ }
	
	/////////////////////////////////////////////////////////////////////// 
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/* Devuelve en 'registro' el registro cuya posicion en el archivo es 
		 * numReg.
		 */
		char ArchivoDatosRegistros::leerRegistro(void* registro, unsigned short numReg){return 0;}	
		
		/*Sobre-escribe en la posicion relativa numReg el registro pasado
		 * por parametro.
		 */
		char ArchivoDatosRegistros::escribirRegistro(void* registro, unsigned short numReg){return 0;}
		
		/*Escribe en el archivo de datos el registro pasado por parametro
		 * en la posicion del primer registro libre que encuentre, utilizando
		 * el archivo de control de espacio libre. Si todos los registro
		 * se encuentran ocupados, appendea al final del archivo.
		 * Devuelve la posicion en donde fue escrito finalmente.
		 */
		 short ArchivoDatosRegistros::escribirRegistro(void* registro){return 0;}
		
		/*Modifica el archivo de control de espacio libre
		 * para que el bloque cuya posicion relativa en el archivo es
		 * numBloque se considere vacio.
		 */
		char ArchivoDatosRegistros::eliminarRegistro(unsigned short numBloque){ return 0;}
