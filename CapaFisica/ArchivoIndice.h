///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.h
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoIndice correspodientes
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

#ifndef ARCHIVOINDICE_H_
#define ARCHIVOINDICE_H_

#include "ArchivoIndicePadre.h"
#include "Clave.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroGriego 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceEnteroGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
			
		/*Esta funcion se encarga de interpretar claves enteras
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave, char* &puntero);
		void copiarClaveNoHoja(Clave* clave, char* &puntero);
		
		/*Esta funcion se encarga de leer Claves de tipo entera
		 * de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		ArchivoIndiceEnteroGriego(int tamNodo,string nombreArchivo);
	
}; //Fin clave ArchivoIndiceEnteroGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroRomano 
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceEnteroRomano: public ArchivoIndiceSecundario
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves enteras
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer cada tipo de Clave de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano(unsigned int tamNodo,string nombreArchivo);
	
}; //Fin clase ArchivoIndiceEnteroRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego 
//		   (Implementa archivo de indices primarios de clave booleana).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceBooleanGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
			
		/*Esta funcion se encarga de interpretar claves booleanas
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave, char* &puntero);
		void copiarClaveNoHoja(Clave* clave, char* &puntero);
		
		/*Esta funcion se encarga de leer Claves de tipo booleanas
		 * de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		ArchivoIndiceBooleanGriego(int tamNodo,string nombreArchivo);
	
}; //Fin clase ArchivoIndiceBooleanGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices primarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCharGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo char
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo char de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano(int tamNodo,string nombreArchivo);
	
}; //Fin clase ArchivoIndiceCharGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharRomano
//		   (Implementa archivo de indices secundarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCharRomano: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo char
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo char de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano(unsigned int tamNodo,string nombreArchivo);
	
}; //Fin clase ArchivoIndiceCharGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortGriego
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceShortGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo short
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo short de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortGriego(unsigned int tamNodo, string nombreArchivo);
	
}; //Fin clase ArchivoIndiceShortGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortRomano
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceShortRomano: public ArchivoIndiceSecundario
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo char
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo char de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortRomano(unsigned int tamNodo, string nombreArchivo);
	
}; //Fin clase ArchivoIndiceShortGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceRealRomano
//		   (Implementa archivo de indices primarios de clave de tipo float).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceRealRomano: public ArchivoIndiceSecundario
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo float
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo float de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceRealRomano(unsigned int tamNodo, string nombreArchivo);
	
}; //Fin clase ArchivoIndiceRealRomano


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaGriego
//		   (Implementa archivo de indices primarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceFechaGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo fecha
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo fecha de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaGriego(unsigned int tamNodo, string nombreArchivo);
	
}; //Fin clase ArchivoIndiceFechaGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaRomano
//		   (Implementa archivo de indices secundarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceFechaRomano: public ArchivoIndiceSecundario
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de tipo fecha
		 *  y copiarla al buffer de escritura
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer clave de tipo fecha de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaRomano(unsigned int tamNodo, string nombreArchivo);
	
}; //Fin clase ArchivoIndiceFechaRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableGriego 
//		   (Implementa archivo de indices primarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceVariableGriego: public ArchivoIndice
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de longitud
		 * variable y copiarla al buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave, char* &puntero);
		void copiarClaveNoHoja(Clave* clave, char* &puntero);
		
		/*Esta funcion se encarga de leer claves variables de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableGriego(int tamNodo, string nombreArchivo);	
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableRomano 
//		   (Implementa archivo de indices secundarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceVariableRomano: public ArchivoIndiceSecundario
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves de longitud
		 * variable y copiarla al buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer claves variables de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableRomano(int tamNodo,string nombreArchivo);
	
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCompuestoGriego 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCompuestoGriego: public ArchivoIndice
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCompuestoGriego(int tamNodo,string nombreArchivo);
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves compuestas
		 * variable y copiarla al buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer claves compuestas de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
};

#endif /*ARCHIVOINDICE_H_*/
