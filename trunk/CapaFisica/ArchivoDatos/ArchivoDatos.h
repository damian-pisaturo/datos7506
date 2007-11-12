////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase ArchivoDatos, 
//		ArchivoDatosBloques y ArchivoDatosRegistros.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef ARCHIVODATOS_H_
#define ARCHIVODATOS_H_

#include "../ArchivoBase/ArchivoBase.h"
#include "../ArchivoEL/ArchivoEL.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoDatos
//			(Implementa primitivas para el manejo de archivos
//           de datos en disco).
///////////////////////////////////////////////////////////////////////////

class ArchivoDatos : public ArchivoBase
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		ArchivoEL* archivoEL;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		/*Crea un archivo de datos de nombre nombreArchivo cuya organizacion logica
		 * es tipoOrg (TipoOrganizacion::REG_FIJOS o TipoOrganizacion::REG_VARIABLES) y cuyo
		 * tamaño de bloque/registro es tamanio.
		 */
		ArchivoDatos(string nombreArchivo, unsigned short tamanio, unsigned char tipoOrg);
		
		/*Cierra el archivo creado
		 */
		virtual ~ArchivoDatos();
	
	///////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////
		ArchivoEL* getArchivoEL()
		{
			return this->archivoEL;
		}	
	
}; /*Fin clase ArchivoDatos*/


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoDatosBloques
//			(Implementa primitivas para el manejo de archivos
//           de datos con organizacion de registros variables en disco).
///////////////////////////////////////////////////////////////////////////
class ArchivoDatosBloques : public ArchivoDatos
{	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoDatosBloques(string nombreArchivo, unsigned short tamBloque);		
		virtual ~ArchivoDatosBloques();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/* Devuelve en 'bloque' el bloque cuya posicion en el archivo es 
		 * numBloque.
		 */
		char leerBloque(void* bloque, unsigned short numBloque);
		
		/*Devuelve en 'bloque' el primer bloque del archivo que disponga 
		 * de una cantidad de espacio libre de, al menos, espacioLibre bytes. 
		 * 
		 * Si encontro algun bloque y no se produjo ningun error, retorna
		 * la posicion relativa del bloque hallado.
		 * Si ningun bloque cumple con el requisito, devuelve RefFisica::BLOQUES_OCUPADOS.
		 */
		short buscarBloque(void* bloque, unsigned int espacioLibre);
		
		/*Sobre-escribe en la posicion relativa numBloque el bloque pasado
		 * por parametro.
		 */
		char escribirBloque(void* bloque, unsigned short numBloque, unsigned short espLibre);
		
		/*Escribe en el archivo de datos el bloque pasado por parametro
		 * en la posicion del primer bloque libre que encuentre, utilizando
		 * el archivo de control de espacio libre. Si todos los bloques
		 * se encuentran ocupados, appendea al final del archivo.
		 * Devuelve la posicion en donde fue escrito finalmente.
		 */
		 short escribirBloque(void* bloque, unsigned short espLibre);
		
		/*Modifica el archivo de control de espacio libre
		 * para que el bloque cuya posicion relativa en el archivo es
		 * numBloque se considere vacio.
		 */
		char eliminarBloque(unsigned short numBloque);
	
}; /*Fin clase ArchivoDatos*/

class ArchivoDatosRegistros : public ArchivoDatos
{	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoDatosRegistros(string nombreArchivo, unsigned short tamReg);		
		~ArchivoDatosRegistros();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/* Devuelve en 'registro' el registro cuya posicion en el archivo es 
		 * numReg.
		 */
		char leerRegistro(void* registro, unsigned short numReg);	
		
		/*Sobre-escribe en la posicion relativa numReg el registro pasado
		 * por parametro.
		 */
		char escribirRegistro(void* registro, unsigned short numReg);
		
		/*Escribe en el archivo de datos el registro pasado por parametro
		 * en la posicion del primer registro libre que encuentre, utilizando
		 * el archivo de control de espacio libre. Si todos los registro
		 * se encuentran ocupados, appendea al final del archivo.
		 * Devuelve la posicion en donde fue escrito finalmente.
		 */
		 short escribirRegistro(void* registro);
		
		/*Modifica el archivo de control de espacio libre
		 * para que el bloque cuya posicion relativa en el archivo es
		 * numBloque se considere vacio.
		 */
		char eliminarRegistro(unsigned short numBloque);
	
}; /*Fin clase ArchivoDatos*/

#endif /*ARCHIVODATOS_H_*/
