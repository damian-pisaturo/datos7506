///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.h
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase NodoBp.
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
		ArchivoIndiceEnteroRomano(int tamNodo,string nombreArchivo);
	
}; //Fin clase ArchivoIndiceEnteroRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego 
//		   (Implementa archivo de indices primarios de clave booleana).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceBooleanGriego: public ArchivoIndice
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
class ArchivoIndiceCharGriego: public ArchivoIndice
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
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices secundarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCharRomano: public ArchivoIndiceSecundario
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
		ArchivoIndiceVariableGriego(int tamNodo,string nombreArchivo);
	
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
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCompuestoGriego(int tamNodo,string nombreArchivo);
	
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceHash 
//		   (Implementa archivo de indices de dispersion).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceHash: public ArchivoIndice
{	
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
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceHash(int tamBucket, string nombreArchivo);	
};


/*
 * Este método se utiliza para hacer una operación de alta en el archivo.
 * Si el registro se puede insertar devuelve OK; si ya existe un registro
 * con la misma clave, devuelve DUPLICATED, y si no hay lugar en el bucket
 * correspondiente para insertarlo, devuelve FAIL.
 */



#endif /*ARCHIVOINDICE_H_*/
