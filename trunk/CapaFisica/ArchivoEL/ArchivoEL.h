////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoEL, ArchivoELFijo y
//		ArchivoELVariable. 
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef ARCHIVOEL_H_
#define ARCHIVOEL_H_

#include "../ArchivoBase/ArchivoBase.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoEL 
//		   (Clase que define el comportamiento del manejo de
//			los archivos de control de espacio libre).
///////////////////////////////////////////////////////////////////////////

class ArchivoEL : public ArchivoBase
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoEL(string nombre, unsigned short tamBloque);
		virtual ~ArchivoEL();
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Añade un nuevo registro de datos de control
		 * al final del archivo de espacio libre.
		 */
		virtual char agregarRegistro(const void* registro);
		
		/* Modifica el registro cuyo numero dentro del 
		 * archivo es numReg por el contenido de 'registro'
		 */
		virtual char modificarRegistro(const void* registro, unsigned short numRegistro);
		
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoELFijo
//		   (Clase que permite manejar archivos de control de espacio
//			libre cuya definicion fisica consiste en un atributo booleano
//			indicando si el bloque se encuentra o no libre para su uso).
///////////////////////////////////////////////////////////////////////////

class ArchivoELFijo : public ArchivoEL
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoELFijo(string nombre);
		virtual ~ArchivoELFijo();
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Busca en el archivo de control el primer bloque en condicion
		 * de libre y devuelve su numero dentro del archivo. Si ningun
		 * bloque se encuentra libre, devuelve ResultadosFisica::BLOQUES_OCUPADOS.
		 */
		short buscarBloqueLibre();	
		
		/*Busca en el archivo de control los dos primeros bloques consecutivos
		 * en condicion de libre y devuelve el numero del primero de ellos
		 * dentro del archivo. Si ningun par de bloques consecutivos se encuentra 
		 * libre, devuelve ResultadosFisica::BLOQUES_OCUPADOS.
		 */		
		short buscarBloqueDobleLibre();
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoELVariable
//		   (Clase que permite manejar archivos de control de espacio
//			libre cuya definicion fisica consiste en un atributo
//			unsigned short indicando espacio libre en el bloque).
///////////////////////////////////////////////////////////////////////////

class ArchivoELVariable : public ArchivoEL
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoELVariable(string nombre);
		virtual ~ArchivoELVariable();
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Busca en el archivo de control el primer bloque que contenga
		 * un numero de bytes libres mayor a espacioRequerido y devuelve 
		 * su posicion dentro del archivo. Si ningun bloque cumple con el 
		 * requisito, devuelve ResFisica::BLOQUES_OCUPADOS.
		 */
		short buscarEspacioLibre(unsigned short espacioRequerido);
		short buscarEspacioLibre();
};

#endif /*ARCHIVOEL_H_*/
